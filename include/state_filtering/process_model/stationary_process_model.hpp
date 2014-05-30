/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014 Max-Planck-Institute for Intelligent Systems,
 *                     University of Southern California
 *    Manuel Wuthrich (manuel.wuthrich@gmail.com)
 *    Jan Issac (jan.issac@gmail.com)
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * @date 05/25/2014
 * @author Manuel Wuthrich (manuel.wuthrich@gmail.com)
 * @author Jan Issac (jan.issac@gmail.com)
 * Max-Planck-Institute for Intelligent Systems, University of Southern California
 */

#ifndef STATE_FILTERING_PROCESS_MODEL_STATIONARY_PROCESS_MODEL_HPP
#define STATE_FILTERING_PROCESS_MODEL_STATIONARY_PROCESS_MODEL_HPP

// eigen
#include <Eigen/Dense>

// boost
#include <boost/shared_ptr.hpp>

// state_filtering
#include <state_filtering/distribution/distribution.hpp>
#include <state_filtering/distribution/features/gaussian_mappable.hpp>
#include <state_filtering/distribution/features/gaussian_sampleable.hpp>
#include <state_filtering/distribution/implementations/gaussian_distribution.hpp>

namespace filter
{

template <typename ScalarType_ = double,
          int VARIABLE_SIZE = Eigen::Dynamic,
          int CONTROL_SIZE = Eigen::Dynamic,
          int RANDOMS_SIZE = Eigen::Dynamic>
class StationaryProcessModel:
        public GaussianMappable<ScalarType_, VARIABLE_SIZE, RANDOMS_SIZE>,
        public GaussianSampleable<GaussianMappable<ScalarType_, VARIABLE_SIZE, RANDOMS_SIZE> >
{
public: /* distribution traits */
    typedef GaussianMappable<ScalarType_, VARIABLE_SIZE, RANDOMS_SIZE> BaseType;

    typedef typename BaseType::ScalarType                           ScalarType;
    typedef typename BaseType::VariableType                         VariableType;
    typedef typename BaseType::RandomsType                          RandomsType;
    typedef Eigen::Matrix<ScalarType, VARIABLE_SIZE, VARIABLE_SIZE> CovarianceType;
    typedef Eigen::Matrix<ScalarType, CONTROL_SIZE, 1>              ControlType;

    virtual ~StationaryProcessModel() { }

    virtual void conditionals(const double& delta_time,
                              const VariableType& state,
                              const ControlType& control) = 0;

    virtual void conditionals(const double& delta_time, const VariableType& state)
    {
        conditionals(delta_time, state, ControlType::Zero(controlSize()));
    }

    virtual int controlSize() const = 0;
};

}

#endif
