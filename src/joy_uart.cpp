#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>
#include <sstream>
#include <serial/serial.h>

class JoystickListener
{
public:
    JoystickListener() : linear_vel_(0.0), angular_vel_(0.0)
    {
        joy_sub_ = nh_.subscribe("joy", 10, &JoystickListener::joyCallback, this);
        cmd_vel_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel", 10);
        
        try
        {
            ser_.setPort("/dev/ttyACM0");  // 시리얼 포트 지정. 필요에 따라 변경.
            ser_.setBaudrate(115200);     // 보드레이트 설정. 필요에 따라 변경.
            serial::Timeout to = serial::Timeout::simpleTimeout(1000);
            ser_.setTimeout(to);
            ser_.open();
        }
        catch (serial::IOException& e)
        {
            ROS_ERROR("Unable to open port.");
        }

        if(ser_.isOpen()){
            ROS_INFO("Serial port initialized.");
        }
    }

    void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
    {
        linear_vel_ = joy->axes[0];
        angular_vel_ = joy->axes[1];
        publishCmdVel();
        sendSerialData();
    }

    void publishCmdVel()
    {
        geometry_msgs::Twist cmd_vel;
        cmd_vel.linear.x = linear_vel_;
        cmd_vel.angular.z = angular_vel_;
        cmd_vel_pub_.publish(cmd_vel);
    }

    void sendSerialData()
    {
        if(ser_.isOpen()){
            std::stringstream ss;
            ss  << linear_vel_ << "," << angular_vel_ << "\n";
            ser_.write(ss.str());
        }
    }

private:
    ros::NodeHandle nh_;
    ros::Subscriber joy_sub_;
    ros::Publisher cmd_vel_pub_;
    double linear_vel_;
    double angular_vel_;
    serial::Serial ser_;
};

int main(int argc, char** argv)
{
    ros::init(argc, argv, "joystick_listener");
    JoystickListener listener;
    ros::spin();
    return 0;
}