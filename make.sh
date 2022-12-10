rm build -r
rm install -r
rm log -r
rosdep update
rosdep install -y -r -i --from-paths src --ignore-src --rosdistro $ROS_DISTRO
#colcon build --allow-overriding obstacle_avoidance_planner
colcon build
source /aichallenge/aichallenge_ws/install/setup.bash
