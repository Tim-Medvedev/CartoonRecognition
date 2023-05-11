Mat sharpen1(const Mat& image) {
	Mat result = image.clone();

	// If greyscale, handle single band
	if (image.channels() == 1) {
		for (int r = 1; r < image.rows - 1; r++) {
			for (int c = 1; c < image.cols - 1; c++) {
				result.at<uchar>(r, c) = saturate_cast<uchar>(5 * image.at<uchar>(r, c)
					- image.at<uchar>(r + 1, c) - image.at<uchar>(r, c + 1)
					- image.at<uchar>(r - 1, c) - image.at<uchar>(r, c - 1));
			}
		}
	}

	// If color, handle three bands
	if (image.channels() == 3) {
		for (int r = 1; r < image.rows - 1; r++) {
			for (int c = 1; c < image.cols - 1; c++) {
				for (int b = 0; b < 3; b++) {
					result.at<Vec3b>(r, c)[b] = saturate_cast<uchar>(5 * image.at<Vec3b>(r, c)[b]
						- image.at<Vec3b>(r + 1, c)[b] - image.at<Vec3b>(r, c + 1)[b]
						- image.at<Vec3b>(r - 1, c)[b] - image.at<Vec3b>(r, c - 1)[b]);
				}
			}
		}
	}

	return result;
}


// sharpen2 - sharpens an image by taking a weighted average of each pixels with its neighbors
// preconditions: image is an greyscale or color byte image
// postconditions: a new image with the same size is returned after sharpening
Mat sharpen2(const Mat& image) {
	Mat result = image.clone();

	if (image.channels() == 1) {
		for (int r = 1; r < image.rows - 1; r++) {

			// Get pointers to necessary rows
			const uchar* previous = image.ptr<const uchar>(r - 1);
			const uchar* current = image.ptr<const uchar>(r);
			const uchar* next = image.ptr<const uchar>(r + 1);
			uchar* output = result.ptr<uchar>(r);

			for (int c = 1; c < image.cols - 1; c++) {
				*output++ = saturate_cast<uchar>(5 * current[c] - current[c - 1] -
					current[c + 1] - previous[c] - next[c]);
			}
		}
	}

	if (image.channels() == 3) {
		for (int r = 1; r < image.rows - 1; r++) {
			// Get pointers to necessary rows
			const uchar* previous = image.ptr<const uchar>(r - 1);
			const uchar* current = image.ptr<const uchar>(r);
			const uchar* next = image.ptr<const uchar>(r + 1);
			uchar* output = result.ptr<uchar>(r);

			for (int c = 3; c < 3 * image.cols - 3; c++) {
				*output++ = saturate_cast<uchar>(5 * current[c] - current[c - 3] -
					current[c + 3] - previous[c] - next[c]);
			}
		}
	}

	return result;
}

// sharpen3 - sharpens an image by taking a weighted average of each pixels with its neighbors
// preconditions: image is an greyscale or color byte image
// postconditions: a new image with the same size is returned after sharpening
Mat sharpen3(const Mat& image) {
	Mat result = image.clone();

	if (image.channels() == 1) {
		// Create iterators
		Mat_<uchar>::const_iterator in = image.begin<uchar>();
		Mat_<uchar>::iterator out = result.begin<uchar>();

		// Skip first row
		in += image.cols;
		out += image.cols;

		for (int r = 1; r < image.rows - 1; r++) {
			// Skip first column
			in++;
			out++;
			for (int c = 1; c < image.cols - 1; c++, in++, out++) {
				*out = saturate_cast<uchar>(5 * *in - *(in - 1) - *(in + 1) -
					*(in + image.cols) - *(in - image.cols));

			}
			// Skip last column
			in++;
			out++;
		}
	}

	if (image.channels() == 3) {
		// Create iterators
		Mat_<Vec3b>::const_iterator in = image.begin<Vec3b>();
		Mat_<Vec3b>::iterator out = result.begin<Vec3b>();

		// Skip first row
		in += image.cols;
		out += image.cols;

		for (int r = 1; r < image.rows - 1; r++) {
			// Skip first column
			in++;
			out++;
			for (int c = 1; c < image.cols - 1; c++, in++, out++) {
				for (int b = 0; b < 3; b++) {
					(*out)[b] = saturate_cast<uchar>(5 * (*in)[b] - (*(in - 1))[b] - (*(in + 1))[b] -
						(*(in + image.cols))[b] - (*(in - image.cols))[b]);
				}
			}
			// Skip last column
			in++;
			out++;
		}
	}

	return result;
}
