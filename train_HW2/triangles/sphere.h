#pragma once

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#define FPUSH_VTX3(p,vx,vy,vz)	do { p.push_back(vx); p.push_back(vy); p.push_back(vz); } while(0)
#define FSET_VTX3(vx,vy,vz, valx,valy,valz)	do { vx=(float)(valx); vy=(float)(valy); vz=(float)(valz); } while(0)

void get_sphere_3d(std::vector<GLfloat>& p, float r, int lats, int longs, std::vector<GLfloat>* colors = NULL) {
	float r2 = r * 2;
	float vx0, vy0, vz0, vx1, vy1, vz1;
	float vx2, vy2, vz2, vx3, vy3, vz3;

	double y0 = 1.0;	// y0 = cos(theta0) at theta0 = 0
	double st0 = 0.0;	// ct0 = sin(theta0) at theta0 = 0

	for (int i = 1; i <= lats; ++i) {
		double theta1 = M_PI * i / lats;
		double y1 = cos(theta1);
		double st1 = sin(theta1);

		// Compute the first two points.
		FSET_VTX3(vx0, vy0, vz0, 0.0f, r*y0, r*st0);
		FSET_VTX3(vx1, vy1, vz1, 0.0f, r*y1, r*st1);

		for (int j = 1; j <= longs; ++j) {
			// Compute the next two points as we move along a horizontal direction.
			double phi = 2 * M_PI * j / longs;
			double cp = cos(phi);
			double sp = sin(phi);
			FSET_VTX3(vx2, vy2, vz2, r*sp*st0, r*y0, r*cp*st0);
			FSET_VTX3(vx3, vy3, vz3, r*sp*st1, r*y1, r*cp*st1);

			if (i < lats) {
				// first triangle (v0 - v1 - v3)
				FPUSH_VTX3(p, vx0, vy0, vz0);
				FPUSH_VTX3(p, vx1, vy1, vz1);
				FPUSH_VTX3(p, vx3, vy3, vz3);

				if (colors) {
					std::vector<GLfloat>& c = *colors;
					FPUSH_VTX3(c, (vx0 + r) / r2, (vy0 + r) / r2, (vz0 + r) / r2);
					FPUSH_VTX3(c, (vx1 + r) / r2, (vy1 + r) / r2, (vz1 + r) / r2);
					FPUSH_VTX3(c, (vx3 + r) / r2, (vy3 + r) / r2, (vz3 + r) / r2);
				}
			}

			if (1 < i) {
				// second triangle (v3 - v2 - v0)
				FPUSH_VTX3(p, vx3, vy3, vz3);
				FPUSH_VTX3(p, vx2, vy2, vz2);
				FPUSH_VTX3(p, vx0, vy0, vz0);

				if (colors) {
					std::vector<GLfloat>& c = *colors;
					FPUSH_VTX3(c, (vx3 + r) / r2, (vy3 + r) / r2, (vz3 + r) / r2);
					FPUSH_VTX3(c, (vx2 + r) / r2, (vy2 + r) / r2, (vz2 + r) / r2);
					FPUSH_VTX3(c, (vx0 + r) / r2, (vy0 + r) / r2, (vz0 + r) / r2);
				}
			}

			FSET_VTX3(vx0, vy0, vz0, vx2, vy2, vz2);
			FSET_VTX3(vx1, vy1, vz1, vx3, vy3, vz3);
		}

		y0 = y1;
		st0 = st1;
	}
}