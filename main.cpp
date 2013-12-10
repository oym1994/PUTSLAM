#include <iostream>
#include "include/Defs/putslam_defs.h"
#include "Grabber/kinect_grabber.h"
#include "Core/Math/CMat44.h"
#include <cmath>

using namespace std;

int main()
{
    try {
        using namespace putslam;

        Grabber* grabber = createGrabberKinect();
        cout << "Current grabber: " << grabber->getName() << endl;
        Point3D::Cloud scene;
        grabber->grab();
        scene = grabber->getCloud();

        CMat44 matrix1;
        matrix1.createTRMatrix(0, M_PI/2, 0, 0.1, 0.2, 0.3);
        matrix1.showMatrix();
        matrix1.inv(&matrix1);
        matrix1.showMatrix();
    }
	catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
