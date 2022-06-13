#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
//#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos, Eigen::Vector3f eye_pos2)
{
    //Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate1;
    translate1 << 1, 0, 0, -eye_pos[0],
                  0, 1, 0, -eye_pos[1],
                  0, 0, 1, -eye_pos[2],
                  0, 0, 0, 1;
    Eigen::Matrix4f translate2;
    translate2 << std::cos(-eye_pos2[0]/180*MY_PI), 0, std::sin(-eye_pos2[0]/180*MY_PI), 0,
                  0, 1, 0, 0,
                  -std::sin(-eye_pos2[0]/180*MY_PI), 0, std::cos(-eye_pos2[0]/180*MY_PI), 0,
                  0, 0, 0, 1;
    Eigen::Matrix4f translate3;
    translate3 << 1, 0, 0, 0,
                  0, std::cos(-eye_pos2[1]/180*MY_PI), -std::sin(-eye_pos2[1]/180*MY_PI), 0,
                  0, std::sin(-eye_pos2[1]/180*MY_PI), std::cos(-eye_pos2[1]/180*MY_PI), 0,
                  0, 0, 0, 1;
    //view = translate * view;

    return translate3 * translate2 * translate1;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    Eigen::Matrix4f rotate;
    rotate << std::cos(rotation_angle / 180.0 * MY_PI), -std::sin(rotation_angle / 180.0 * MY_PI), 0, 0,
              std::sin(rotation_angle / 180.0 * MY_PI), std::cos(rotation_angle / 180.0 * MY_PI), 0, 0,
              0, 0, 1, 0,
              0, 0, 0, 1;
    
    model = rotate * model;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function


    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    Eigen::Matrix4f persp;
    Eigen::Matrix4f translate;
    Eigen::Matrix4f scale;

    //if in perspective_projection zNear_projection_plane do not change && z_axis do not change && could *{multiply} a numble:
    persp << zNear, 0, 0, 0,
             0, zNear, 0, 0,
             0, 0, zNear + zFar, -zNear * zFar,
             0, 0, 1, 0;
    
    float halfAngle = eye_fov / 2 / 180 * MY_PI;
    float h = -zNear * tan(halfAngle);
    float w = h * aspect_ratio;

    translate << 1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, -(zNear + zFar) / 2,
                 0, 0, 0, 1;

    scale << 1 / w, 0, 0, 0,
             0, 1 / h, 0, 0,
             0, 0, 1 / (zNear - zFar), 0,
             0, 0, 0, 1;

    Eigen::Matrix4f projection = scale * translate * persp;
    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};
    Eigen::Vector3f eye_pos2 = {0, 0, 0};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    std::vector<Eigen::Vector3f> pos1{{4, -4, 0}, {0, -4, 4}, {-4, -4, 0}};

    std::vector<Eigen::Vector3i> ind1{{0, 1, 2}};

    auto pos_id1 = r.load_positions(pos1);
    auto ind_id1 = r.load_indices(ind1);

    int key = 0;
    int frame_count = 0;
    float x_eye_pos = 0, y_eye_pos = 0, z_eye_pos = 5, eye_pos3 = 0, eye_pos4 = 0;
    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos, eye_pos2));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        r.draw(pos_id1, ind_id1, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos, eye_pos2));
        r.set_projection(get_projection_matrix(120, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        r.draw(pos_id1, ind_id1, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == '+') {
            angle += 10;
        }
        else if (key == '-') {
            angle -= 10;
        }
        else if (key == 'w') {
            
            z_eye_pos -= 5;
        }
        else if (key == 's') {
            z_eye_pos += 5;
        }
        else if (key == 'a') {
            x_eye_pos -= 5;
        }
        else if (key == 'd') {
            x_eye_pos += 5;
        }
        else if (key == 'j') { //left
            eye_pos3 += 10.1;
            std::cout<< "eye_pos3 += 5;";
        }
        else if (key == 'l') {
            eye_pos3 -= 10.1;
        }
        else if (key == 'i') { //up
            eye_pos4 += 10.1;
        }
        else if (key == 'k') {
            eye_pos4 -= 10.1;
        }
        eye_pos = {x_eye_pos, y_eye_pos, z_eye_pos};
        eye_pos2 = {eye_pos3, eye_pos4, 0};

    }

    return 0;
}
