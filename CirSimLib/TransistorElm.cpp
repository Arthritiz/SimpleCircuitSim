#include "TransistorElm.hpp"

TransistorElm::TransistorElm(CirSim* sim, std::string modelName, std::vector<int> nodes): CircuitElm(sim, std::move(nodes)) {
    this->CircuitElm::baseAlloc();
    
    auto iter = TransistorElm::modelSpecs.find(modelName);
    if (iter == TransistorElm::modelSpecs.end()) {
        throw;
    }
    this->spec = iter->second;
    
    this->vcrit = TransistorElm::VT * std::log(TransistorElm::VT/(std::sqrt(2)*this->spec.satCur));
}

double TransistorElm::limitStep(double vnew, double vold) {
    double arg;
    
    if ((vnew > this->vcrit) && (abs(vnew - vold) > (TransistorElm::VT + TransistorElm::VT))) {
        if (vold > 0) {
            arg = 1 + (vnew - vold)/TransistorElm::VT;
            if (arg > 0) {
                vnew = vold + TransistorElm::VT * std::log(arg);
            } else {
                vnew = this->vcrit;
            }
        } else {
            vnew = TransistorElm::VT * std::log(vnew/TransistorElm::VT);
        }
        
        this->cirSim->setConverged(false);
    }
    
    return vnew;
}

void TransistorElm::doStep() {
    double vbc = this->spec.pnp * (volts[0] - volts[1]);
    double vbe = this->spec.pnp * (volts[0] - volts[2]);
    
    if (abs(vbc - lastVbc) > 0.01 || abs(vbe - lastVbe) > 0.01) {
        this->cirSim->setConverged(false);
    }
    
    double gmin = 1e-12;
    
    vbc = this->limitStep(vbc, lastVbc);
    vbe = this->limitStep(vbe, lastVbe);
    this->lastVbc = vbc;
    this->lastVbe = vbe;
    
    double csat = this->spec.satCur;
    double c2 = this->spec.BEleakCur;
    double c4 = this->spec.BCleakCur;
    double vte = this->spec.leakBEemissionCoeff*TransistorElm::VT;
    double vtc = this->spec.leakBCemissionCoeff*TransistorElm::VT;
    double oik = this->spec.invRollOffF;
    double oikr = this->spec.invRollOffR;
    
    double vtn = TransistorElm::VT*this->spec.emissionCoeffF;
    double evbe, cbe, gbe, cben, gben, evben, evbc, cbc, gbc, cbcn, gbcn, evbcn;
    double qb, dqbdve, dqbdvc, q2, sqarg, arg;
    
    if(vbe > -5*vtn){
        evbe = std::pow(std::exp(1.0), vbe/vtn);
        cbe = csat*(evbe-1)+gmin*vbe;
        gbe = csat*evbe/vtn+gmin;
        if (c2 == 0) {
            cben = 0;
            gben = 0;
        } else {
            evben = std::pow(std::exp(1.0), vbe/vte);
            cben = c2*(evben-1);
            gben = c2*evben/vte;
        }
    } else {
        gbe = -csat/vbe+gmin;
        cbe = gbe*vbe;
        gben = -c2/vbe;
        cben = gben*vbe;
    }
    
    vtn = TransistorElm::VT*this->spec.emissionCoeffR;
    if(vbc > -5*vtn) {
        evbc = std::pow(std::exp(1.0), vbc/vtn);
        cbc = csat*(evbc-1) + gmin*vbc;
        gbc = csat*evbc/vtn + gmin;
        if (c4 == 0) {
            cbcn = 0;
            gbcn = 0;
        } else {
            evbcn = std::pow(std::exp(1.0), vbc/vtc);
            cbcn = c4*(evbcn-1);
            gbcn = c4*evbcn/vtc;
        }
    } else {
        gbc = -csat/vbc+gmin;
        cbc = gbc*vbc;
        gbcn = -c4/vbc;
        cbcn = gbcn*vbc;
    }
    
     // base charge terms
    double q1 = 1/(1 - this->spec.invEarlyVoltF*vbc - this->spec.invEarlyVoltR*vbe);
    if(oik == 0 && oikr == 0) {
        qb = q1;
        dqbdve = q1*qb*this->spec.invEarlyVoltR;
        dqbdvc = q1*qb*this->spec.invEarlyVoltF;
    } else {
        q2 = oik*cbe+oikr*cbc;
        arg = std::max(0.0, 1 + 4*q2);
        sqarg = 1;
        if(arg != 0) sqarg = std::sqrt(arg);
        qb = q1*(1+sqarg)/2;
        dqbdve = q1*(qb*this->spec.invEarlyVoltR + oik*gbe/sqarg);
        dqbdvc = q1*(qb*this->spec.invEarlyVoltF + oikr*gbc/sqarg);
    }
    
    double cc = 0;
    double cex = cbe;
    double gex = gbe;
    /*
     *   determine dc incremental conductances
     */
    cc = cc + (cex-cbc)/qb - cbc/this->spec.betaR - cbcn;
    double cb = cbe/this->spec.beta + cben + cbc/this->spec.betaR + cbcn;
    
    // get currents
    this->ic = this->spec.pnp * cc;
    this->ib = this->spec.pnp * cb;
    this->ie = this->spec.pnp * (-cc-cb);
    
    double gpi = gbe/this->spec.beta + gben;
    double gmu = gbc/this->spec.betaR + gbcn;
    double go = (gbc + (cex-cbc)*dqbdvc/qb)/qb;
    double gm = (gex - (cex-cbc)*dqbdve/qb)/qb-go;

    double ceqbe = this->spec.pnp * (cc + cb - vbe * (gm + go + gpi) + vbc * go);
    double ceqbc = this->spec.pnp * (-cc + vbe * (gm + go) - vbc * (gmu + go));
    
    this->cirSim->stampMatrix(nodes[1], nodes[1], gmu+go);
    this->cirSim->stampMatrix(nodes[1], nodes[0], -gmu+gm);
    this->cirSim->stampMatrix(nodes[1], nodes[2], -gm-go);
    this->cirSim->stampMatrix(nodes[0], nodes[0], gpi+gmu);
    this->cirSim->stampMatrix(nodes[0], nodes[2], -gpi);
    this->cirSim->stampMatrix(nodes[0], nodes[1], -gmu);
    this->cirSim->stampMatrix(nodes[2], nodes[0], -gpi-gm);
    this->cirSim->stampMatrix(nodes[2], nodes[1], -go);
    this->cirSim->stampMatrix(nodes[2], nodes[2], gpi+gm+go);
    
    this->cirSim->stampRightSide(nodes[0], -ceqbe-ceqbc);
    this->cirSim->stampRightSide(nodes[1], ceqbc);
    this->cirSim->stampRightSide(nodes[2], ceqbe);
}
