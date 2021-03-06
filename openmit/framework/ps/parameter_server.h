/*!
 *  Copyright (c) 2016 by Contributors
 *  \file basic_formula.h
 *  \brief basic match formula, such as sigmoid, distance etc.
 *  \author ZhouYong
 */
#ifndef OPENMIT_FRAMEWORK_PS_PARAMETER_SERVER_H_
#define OPENMIT_FRAMEWORK_PS_PARAMETER_SERVER_H_

#include "openmit/learner/mi_learner.h"
#include "openmit/framework/ps/scheduler.h"
#include "openmit/framework/ps/server.h"
#include "openmit/framework/ps/worker.h"

namespace mit {
/*! 
 * \brief parameter server computational platform parameter
 */
class PSParam : public dmlc::Parameter<PSParam> {
  public:
    /*! framework */
    std::string framework;

    /*! declare parameter field */
    DMLC_DECLARE_PARAMETER(PSParam) {
      DMLC_DECLARE_FIELD(framework).set_default("ps");
    }
}; // class PSParam

/*!
 * \brief parameter server framework for
 *        distributed machine learning tasks.
 */
class PS : public MILearner {
  public:
    /*! \brief constructor */
    PS(const mit::KWArgs & kwargs);

    /*! \brief destructor */
    ~PS() {}

    /*! \brief get parameter server object */
    static PS * Get(const mit::KWArgs & kwargs) {
      return new PS(kwargs);
    }
    /*! \brief running */
    void Run() override;
  private:
    /*! \brief scheduler launcher */
    void LaunchScheduler();
    /*! \brief server launcher */
    void LaunchServer();
    /*! \brief worker launcher */
    void LaunchWorker();
  private:
    /*! \brief */
    mit::KWArgs kwargs_;
    /*! \brief */
    PSParam param_;
}; // class PS
} // namespace mit

#endif // OPENMIT_FRAMEWORK_PS_PARAMETER_SERVER_H_
