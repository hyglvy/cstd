
#include "cstd.h"
#include <fstream>
#include <vector>
#include <algorithm>
//#include "facelib/Seeta/test_SeetaFaceEngine.inl"
#include "facelib/Seeta/FaceDetection.inl"
#include "facelib/Seeta/FaceAlignment.inl"
#include "facelib/Seeta/FaceIdentification.inl"

#if 1
#define FACE_IDENTIFICATION_MODEL_PATH  "D:/data/model/seeta_fr_v1.0.bin"
#else
#define FACE_IDENTIFICATION_MODEL_PATH  "D:/fr_v1.0.bin"
#endif

int ExtractFaceFeature(int cols, int rows, unsigned char* data, int c, const float* points, float* gallery_fea) {
  ImageData gallery_img_data_color(data, cols, rows, c);
	static FaceIdentification* face_recognizer = NULL;
  if (!face_recognizer) {
    face_recognizer  = new FaceIdentification(FACE_IDENTIFICATION_MODEL_PATH);
  }

  if (face_recognizer) {
	  FacialLandmark gallery_points[5];
    for (int i=0; i<5; ++i) {
      gallery_points[i].x = points[i];
      gallery_points[i].y = points[i+5];
    }
	  face_recognizer->ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
  }
  return 0;
}

float GetSimilarity(const float* fc1, const float* fc2, int dim) {
	return simd_dot(fc1, fc2, dim)
		/ (sqrt(simd_dot(fc1, fc1, dim))
			* sqrt(simd_dot(fc2, fc2, dim)));
}
float CalcSimilarity(const float* fc1, const float* fc2, int dim) {
	return simd_dot(fc1, fc2, dim)
		/ (sqrt(simd_dot(fc1, fc1, dim))
			* sqrt(simd_dot(fc2, fc2, dim)));
}


#include "lib.inl"

