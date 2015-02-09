/*
 * This is part of the FL library, a C++ Bayesian filtering library
 * (https://github.com/filtering-library)
 *
 * Copyright (c) 2014 Jan Issac (jan.issac@gmail.com)
 * Copyright (c) 2014 Manuel Wuthrich (manuel.wuthrich@gmail.com)
 *
 * Max-Planck Institute for Intelligent Systems, AMD Lab
 * University of Southern California, CLMC Lab
 *
 * This Source Code Form is subject to the terms of the MIT License (MIT).
 * A copy of the license can be found in the LICENSE file distributed with this
 * source code.
 */

/**
 * \date January 2015
 * \author Jan Issac (jan.issac@gmail.com)
 */

#include "dev_test_example.hpp"


#include <std_msgs/Float32.h>


int main (int argc, char **argv)
{
    /* ############################## */
    /* # Setup ros                  # */
    /* ############################## */
    ros::init(argc, argv, "dev_test_example");
    ros::NodeHandle nh("~");

    /* ############################## */
    /* # Setup object and tracker   # */
    /* ############################## */
    DevTestExample tracker(nh);
    DevTestExample::Observation y(tracker.obsrv_model_->observation_dimension(), 1);

    /* ############################## */
    /* # Images                     # */
    /* ############################## */
    fl::ImagePublisher ip(nh);

    Eigen::MatrixXd image_vector(tracker.obsrv_model_->observation_dimension(), 1);

    std::cout << ">> initial state " << std::endl;
    std::cout << tracker.state_distr_.mean().transpose() << std::endl;

    std::cout << ">> setup: " << std::endl;    
    std::cout << ">> state dimension: "
              << tracker.process_model_->state_dimension() << std::endl;
    std::cout << ">> noise dimension: "
              << tracker.process_model_->noise_dimension() << std::endl;
    std::cout << ">> obsrv dimension: "
              << tracker.obsrv_model_->observation_dimension() << std::endl;
    std::cout << ">> obsrv noise dimension: "
              << tracker.obsrv_model_->noise_dimension() << std::endl;
    std::cout << ">> obsrv state dimension: "
              << tracker.obsrv_model_->state_dimension() << std::endl;

    int max_cycles;
    int cycle = 0;
    ri::ReadParameter("max_cycles", max_cycles, nh);


    std_msgs::Float32 x_msg;
    ros::Publisher pub_x_gt
        = nh.advertise<std_msgs::Float32>("/dev_test_example/x_gt", 10000);
    ros::Publisher pub_x_e
        = nh.advertise<std_msgs::Float32>("/dev_test_example/x_e", 10000);

    ros::Publisher pub_sigma_x_p
        = nh.advertise<std_msgs::Float32>("/dev_test_example/sigma_x_p", 10000);
    ros::Publisher pub_sigma_x_n
        = nh.advertise<std_msgs::Float32>("/dev_test_example/sigma_x_n", 10000);

    ros::Publisher pub_x_avrg
        = nh.advertise<std_msgs::Float32>("/dev_test_example/x_avrg", 10000);

    double ground_truth;


    int error_pixels;
    int cycle_of_error;
    int error_duration;
    double error_value;
    bool gradual_error;
    int gradual_delay;
    int current_error_pixels = 0;
    ri::ReadParameter("cycle_of_error", cycle_of_error, nh);
    ri::ReadParameter("error_duration", error_duration, nh);
    ri::ReadParameter("error_value", error_value, nh);
    ri::ReadParameter("error_pixels", error_pixels, nh);
    ri::ReadParameter("gradual_error", gradual_error, nh);
    ri::ReadParameter("gradual_delay", gradual_delay, nh);


    std::vector<std::pair<std_msgs::Float32, ros::Publisher>> pubs_y;
    std::vector<std::pair<std_msgs::Float32, ros::Publisher>> pubs_b;
    for (int i = 0; i < std::min(tracker.pixels, 10); ++i)
    {
        pubs_y.push_back(
            std::make_pair<std_msgs::Float32, ros::Publisher>(
                std_msgs::Float32(),
                nh.advertise<std_msgs::Float32>(
                    std::string("/dev_test_example/y") + std::to_string(i),
                    10000)));

        pubs_b.push_back(
            std::make_pair<std_msgs::Float32, ros::Publisher>(
                std_msgs::Float32(),
                nh.advertise<std_msgs::Float32>(
                    std::string("/dev_test_example/b") + std::to_string(i),
                    10000)));
    }

    double t = 0.;
    double step = 2. * M_PI / double(max_cycles);


    double w_sigma;
    double sim_w_sigma;
    ri::ReadParameter("w_sigma", w_sigma, nh);
    ri::ReadParameter("sim_w_sigma", sim_w_sigma, nh);
    ri::ReadParameter("k", tracker.obsrv_model_->pixel_observation_model()->k_, nh);
    fl::Gaussian<DevTestExample::Observation> g(tracker.obsrv_model_->observation_dimension());
    g.covariance(g.covariance() * sim_w_sigma * sim_w_sigma);

    std::cout << ">> ready to run ..." << std::endl;

    while(ros::ok())
    {
        ground_truth = std::sin(t);

        t += step;
        y.setOnes();
        y = y * ground_truth + g.sample();
        for (int i = 0; i < tracker.pixels; ++i)
        {
            y(2 * i + 1) = y(2 * i) * y(2 * i);
        }

        if (cycle_of_error < cycle && cycle_of_error + error_duration > cycle)
        {
            if (!gradual_error)
            {
                current_error_pixels = error_pixels;
            }
            else
            {
                current_error_pixels = (cycle - cycle_of_error) / gradual_delay + 1;
                current_error_pixels = std::min(current_error_pixels, error_pixels);
            }

            for (int i = 0; i < current_error_pixels && i < tracker.pixels; ++i)
            {
                y(2 * i) = error_value;
                y(2 * i + 1) = y(2 * i) * y(2 * i);
            }
        }

        for (int i = 0; i < std::min(tracker.pixels, 10); ++i)
        {
            pubs_y[i].first.data = y(2 * i);
            pubs_y[i].second.publish(pubs_y[i].first);
        }

        tracker.filter(y);

        // a
        x_msg.data = tracker.state_distr_.mean()(0);
        pub_x_e.publish(x_msg);

        // sigma_a plus
        x_msg.data = tracker.state_distr_.mean()(0)
                     + std::sqrt(tracker.state_distr_.covariance()(0,0));
        pub_sigma_x_p.publish(x_msg);

        // sigma_a minus
        x_msg.data = tracker.state_distr_.mean()(0)
                     - std::sqrt(tracker.state_distr_.covariance()(0,0));
        pub_sigma_x_n.publish(x_msg);


        // b
        for (int i = 0; i < std::min(tracker.pixels, 10); ++i)
        {
            double b = tracker.state_distr_.mean()(1 + i);
            pubs_b[i].first.data = tracker.obsrv_model_->pixel_observation_model()->sigma(b);
            pubs_b[i].second.publish(pubs_b[i].first);
        }

        // y
        x_msg.data = y.mean();
        pub_x_avrg.publish(x_msg);

        // ground truth
        x_msg.data = ground_truth;
        pub_x_gt.publish(x_msg);                        


        ip.publish(y, "/dev_test_tracker/observation", 1, y.rows());

        if (max_cycles > 0 && cycle++ > max_cycles) break;

        ros::Duration(0.01).sleep();
        ros::spinOnce();
    }

    return 0;
}