#pragma once

#include "ofMain.h"

class ofxHomography {
public:
	
	/*
	 * Homography Functions adapted from:
	 * http://www.openframeworks.cc/forum/viewtopic.php?p=22611
	 * Author: arturo castro
	 */
	static void gaussian_elimination(float *input, int n){
		// ported to c from pseudocode in
		// http://en.wikipedia.org/wiki/Gaussian_elimination
		
		float * A = input;
		int i = 0;
		int j = 0;
		int m = n-1;
		while (i < m && j < n){
			// Find pivot in column j, starting in row i:
			int maxi = i;
			for(int k = i+1; k<m; k++){
				if(fabs(A[k*n+j]) > fabs(A[maxi*n+j])){
					maxi = k;
				}
			}
			if (A[maxi*n+j] != 0){
				//swap rows i and maxi, but do not change the value of i
				if(i!=maxi)
					for(int k=0;k<n;k++){
						float aux = A[i*n+k];
						A[i*n+k]=A[maxi*n+k];
						A[maxi*n+k]=aux;
					}
				//Now A[i,j] will contain the old value of A[maxi,j].
				//divide each entry in row i by A[i,j]
				float A_ij=A[i*n+j];
				for(int k=0;k<n;k++){
					A[i*n+k]/=A_ij;
				}
				//Now A[i,j] will have the value 1.
				for(int u = i+1; u< m; u++){
					//subtract A[u,j] * row i from row u
					float A_uj = A[u*n+j];
					for(int k=0;k<n;k++){
						A[u*n+k]-=A_uj*A[i*n+k];
					}
					//Now A[u,j] will be 0, since A[u,j] - A[i,j] * A[u,j] = A[u,j] - 1 * A[u,j] = 0.
				}
				
				i++;
			}
			j++;
		}
		
		//back substitution
		for(int i=m-2;i>=0;i--){
			for(int j=i+1;j<n-1;j++){
				A[i*n+m]-=A[i*n+j]*A[j*n+m];
				//A[i*n+j]=0;
			}
		}
	}
	
	static std::vector<glm::vec3> rectangleToCorners(const ofRectangle& r){
		std::vector<glm::vec3> v;
		v.resize(4);
		v[0] = r.getTopLeft();
		v[1] = r.getTopRight();
		v[2] = r.getBottomRight();
		v[3] = r.getBottomLeft();
		return v;
	}
	
	template <class T, class C>
	static void findHomography(const std::vector<T> & src, const std::vector<C> & dst, glm::mat4& homography){
		
		// create the equation system to be solved
		// src and dst must implement [] operator for point access
		//
		// from: Multiple View Geometry in Computer Vision 2ed
		//       Hartley R. and Zisserman A.
		//
		// x' = xH
		// where H is the homography: a 3 by 3 matrix
		// that transformed to inhomogeneous coordinates for each point
		// gives the following equations for each point:
		//
		// x' * (h31*x + h32*y + h33) = h11*x + h12*y + h13
		// y' * (h31*x + h32*y + h33) = h21*x + h22*y + h23
		//
		// as the homography is scale independent we can let h33 be 1 (indeed any of the terms)
		// so for 4 points we have 8 equations for 8 terms to solve: h11 - h32
		// after ordering the terms it gives the following matrix
		// that can be solved with gaussian elimination:
		if(src.size() >= 4 && dst.size() >= 4){
		float P[8][9]={
			{-src[0].x, -src[0].y, -1,   0,   0,  0, src[0].x*dst[0].x, src[0].y*dst[0].x, -dst[0].x }, // h11
			{  0,   0,  0, -src[0].x, -src[0].y, -1, src[0].x*dst[0].y, src[0].y*dst[0].y, -dst[0].y }, // h12
			
			{-src[1].x, -src[1].y, -1,   0,   0,  0, src[1].x*dst[1].x, src[1].y*dst[1].x, -dst[1].x }, // h13
			{  0,   0,  0, -src[1].x, -src[1].y, -1, src[1].x*dst[1].y, src[1].y*dst[1].y, -dst[1].y }, // h21
			
			{-src[2].x, -src[2].y, -1,   0,   0,  0, src[2].x*dst[2].x, src[2].y*dst[2].x, -dst[2].x }, // h22
			{  0,   0,  0, -src[2].x, -src[2].y, -1, src[2].x*dst[2].y, src[2].y*dst[2].y, -dst[2].y }, // h23
			
			{-src[3].x, -src[3].y, -1,   0,   0,  0, src[3].x*dst[3].x, src[3].y*dst[3].x, -dst[3].x }, // h31
			{  0,   0,  0, -src[3].x, -src[3].y, -1, src[3].x*dst[3].y, src[3].y*dst[3].y, -dst[3].y }, // h32
		};
		
		gaussian_elimination(&P[0][0],9);
		
		// gaussian elimination gives the results of the equation system
		// in the last column of the original matrix.
		// opengl needs the transposed 4x4 matrix:
		float aux_H[]={ P[0][8],P[3][8],0,P[6][8],	// h11  h21 0 h31
			P[1][8],P[4][8],0,P[7][8],	// h12  h22 0 h32
			0      ,      0,0,0,		// 0    0   0 0
			P[2][8],P[5][8],0,1};		// h13  h23 0 h33
		
//		for(int i=0;i<16;i++) homography[i] = aux_H[i];
		homography = glm::make_mat4(aux_H);
		}else{
			ofLogError("ofxHomography::findHomography") << "Source or destination wrong size!. must be equat to 4.";
		}
	}
	
	template <class T, class C>
	static glm::mat4 findHomography(const T& source, const C& destination){
		glm::mat4 homography;
		findHomography(source, destination, homography);
		return homography;
	}
	static glm::vec3 toDestinationCoordinates(const glm::vec3& point, const glm::mat4& homography){
		glm::vec4 destPoint =  homography * glm::vec4(point, 1.0);
		return  glm::vec3(destPoint) / destPoint.w;
	}
	static glm::vec3 toSourceCoordinates(const glm::vec3& point, const glm::mat4& homography){
		glm::vec4 source = glm::vec4(point, 1.0) * glm::inverse(homography);
		return  glm::vec3(source) / source.w;
	}

};
