#include "rclcpp/rclcpp.hpp"

#include "autoware_auto_system_msgs/msg/autoware_state.hpp"
#include "autoware_auto_vehicle_msgs/msg/engage.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include "std_msgs/msg/string.hpp"

#include <cstdio>

class SampleCode : public rclcpp::Node
{
public:
  SampleCode() : Node("sample_code")
  {
    // publisher
    engage_publisher =
      this->create_publisher<autoware_auto_vehicle_msgs::msg::Engage>("/autoware/engage", 1);
    goal_pos_publisher =
      this->create_publisher<geometry_msgs::msg::PoseStamped>("/planning/mission_planning/goal", 1);
    check_pos_publisher = this->create_publisher<geometry_msgs::msg::PoseStamped>(
      "/planning/mission_planning/checkpoint", 1);

    init_pos_publisher =
      this->create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>("/initialpose", 1);

    // subscriber
    state_subscriber = this->create_subscription<autoware_auto_system_msgs::msg::AutowareState>(
      "/autoware/state", 1, std::bind(&SampleCode::stateCallback, this, std::placeholders::_1));

    // msg
    check_info_msg = setUpMsgCheckInfo();
    goal_info_msg = setUpMsgGoalInfo();
    engage_msg = setUpMsgEngage();

    initial_info_msg = setUpMsgInitialInfo();

    // other
    stepCounter = 0;
    state = 0;
  }

private:
  void stateCallback(const autoware_auto_system_msgs::msg::AutowareState msg)
  {
    state = msg.state;
    // std::cout << state << std::endl;

    // ego vehicle base
    if (state == 1 && stepCounter == 0) {
      init_pos_publisher ->publish(initial_info_msg);
      std::cout << "Publish the Initial Point Info" << std::endl;
    // Goal Pos Pub --> Check Point Pub --> Engage Pub
    }
    //  Publish the Goal Point Info
    if (state == 2 && stepCounter == 0) {
      stepCounter++;

      // publish
      goal_pos_publisher->publish(goal_info_msg);
      std::cout << "Publish the Goal Point Info" << std::endl;

      // Publish the Check Point Info
    } else if (state == 4 && stepCounter == 1) {
      stepCounter++;

      // publish
      check_pos_publisher->publish(check_info_msg);
      std::cout << "Publish the Check Point Info" << std::endl;

      // Publish the Engage, Vehicle drive
    } else if (state == 4 && stepCounter == 2) {
      stepCounter++;
      // publish
      std::cout << "Publish the Engage" << std::endl;
      engage_publisher->publish(engage_msg);
    } else if (state == 3) {
      std::cout << "Planning....." << std::endl;
    }
  }

  // setup msg(engage)
  autoware_auto_vehicle_msgs::msg::Engage setUpMsgEngage()
  {
    auto msg = autoware_auto_vehicle_msgs::msg::Engage();
    msg.engage = true;
    return msg;
  }

  // setup msg(initial info) 2022/12/5
  geometry_msgs::msg::PoseWithCovarianceStamped setUpMsgInitialInfo()
  {
    auto msg = geometry_msgs::msg::PoseWithCovarianceStamped();
    msg.header.frame_id = "map";
    msg.pose.pose.position.x = 81379.2109375;
    msg.pose.pose.position.y = 49917.625;
    msg.pose.pose.position.z = 0.0;
    msg.pose.pose.orientation.x = 0.0;
    msg.pose.pose.orientation.y = 0.0;
    msg.pose.pose.orientation.z = 0.26281721767320854;
    msg.pose.pose.orientation.w = 0.9648456405531992;
    msg.pose.covariance= {
        0.0225, 0.0,   0.0,   0.0,      0.0,      0.0,
        0.0,   0.0225, 0.0,   0.0,      0.0,      0.0,
        0.0,   0.0,   0.0225, 0.0,      0.0,      0.0,
        0.0,   0.0,   0.0,   0.000625, 0.0,      0.0,
        0.0,   0.0,   0.0,   0.0,      0.000625, 0.0,
        0.0,   0.0,   0.0,   0.0,      0.0,      0.000625,
    };
    return msg;
  }

  // setup msg(goal info)
  geometry_msgs::msg::PoseStamped setUpMsgGoalInfo()
  {
    auto msg = geometry_msgs::msg::PoseStamped();
    msg.header.frame_id = "map";
    msg.pose.position.x =81527.703125;
    msg.pose.position.y =50360.001875;
    msg.pose.position.z = 0.0;
    msg.pose.orientation.x = 0.0;
    msg.pose.orientation.y = 0.0;
    msg.pose.orientation.z = 0.7493972074440077;
    msg.pose.orientation.w =0.662120703101121;
    return msg;
  }

  // setup msg(check info)

  geometry_msgs::msg::PoseStamped setUpMsgCheckInfo()
  {
    auto msg = geometry_msgs::msg::PoseStamped();
    msg.header.frame_id = "map";
    msg.pose.position.x = 81398.4375;
    msg.pose.position.y = 49917.625;
    msg.pose.position.z = 0.0;
    msg.pose.orientation.x = 0.0;
    msg.pose.orientation.y = 0.0;
    msg.pose.orientation.z = 0.26281721767320854;
    msg.pose.orientation.w = 0.9648456405531992;
    return msg;
  }

  // publisher
  rclcpp::Publisher<autoware_auto_vehicle_msgs::msg::Engage>::SharedPtr engage_publisher;
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr goal_pos_publisher;
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr check_pos_publisher;
  rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr init_pos_publisher;

  // subscriber
  rclcpp::Subscription<autoware_auto_system_msgs::msg::AutowareState>::SharedPtr state_subscriber;

  // msg
  geometry_msgs::msg::PoseStamped check_info_msg = geometry_msgs::msg::PoseStamped();
  geometry_msgs::msg::PoseStamped goal_info_msg = geometry_msgs::msg::PoseStamped();
  autoware_auto_vehicle_msgs::msg::Engage engage_msg = autoware_auto_vehicle_msgs::msg::Engage();

  geometry_msgs::msg::PoseWithCovarianceStamped initial_info_msg = geometry_msgs::msg::PoseWithCovarianceStamped();
  // other
  int stepCounter;
  int state;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SampleCode>());
  rclcpp::shutdown();
  return 0;
}
