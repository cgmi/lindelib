#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include "linde.h"
#include "Convolution.h"

#define MASKED -999999


/**
* @author by Thomas Lindemeier
* University of Konstanz - 
* Department of Computer Graphics and Media Design
*
* @date 20.11.2013
* 	
*/
namespace linde
{
	using namespace std;

	template <class T>
	class Morphology
	{
	private:
        cv::Mat_<bool> m_sm;
	public:
		Morphology();
		~Morphology();

        cv::Mat_<bool> getStructuringElement() const;

        void createStructuringCircle(int radius, T v);

        void erode(const  cv::Mat_<T> & input,  cv::Mat_<T> & output) const;
        void dilate(const  cv::Mat_<T> & input, cv::Mat_<T> & output) const;

        void open(const cv::Mat_<T> & input, cv::Mat_<T> & output) const;
        void close(const cv::Mat_<T> & input, cv::Mat_<T> & output) const;

        void morphGradient(const cv::Mat_<T> & input, cv::Mat_<T> & output) const;

        void topHatWhite(const cv::Mat_<T> & input, cv::Mat_<T> & output) const;
        void topHatBlack(const cv::Mat_<T> & input, cv::Mat_<T> & output) const;
	};

	template <class T>
	Morphology<T>::Morphology()
	{

	}
	template <class T>
	Morphology<T>::~Morphology()
	{
	}

	template <class T>
    cv::Mat_<bool> Morphology<T>::getStructuringElement() const
	{
		return m_sm;
	}


	template <class T>
    void Morphology<T>::createStructuringCircle(int radius, T v)
	{
        int k_size = radius * 2 + 1;

        m_sm.create(k_size, k_size);
        m_sm.setTo(T(0));

        glm::vec2 center(radius, radius);
		for (int i = 0; i < m_sm.rows; i++)
		{
			for (int j = 0; j < m_sm.cols; j++)
			{
                if (glm::distance(center, glm::vec2(j, i)) <= radius)
				{
                    m_sm(i, j) = v;
				}
			}
		}		
	}

	// shrink white
	template <class T>
    void Morphology<T>::erode(const cv::Mat_<T> & input, cv::Mat_<T> & output) const
	{
        cv::Mat_<T> temp(input.size());
        cv::erode(input, temp, m_sm);
		output = temp;
	}

	// expand white
	template <class T>
    void Morphology<T>::dilate(const cv::Mat_<T> & input, cv::Mat_<T> & output) const
	{
        cv::Mat_<T> temp(input.size());
        cv::dilate(input, temp, m_sm);
		output = temp;

	}

	// dilation of the erosion, remove small white objects
	template <class T>
    void Morphology<T>::open(const cv::Mat_<T> & input, cv::Mat_<T> & output) const
	{
		erode(input, output);
		dilate(output, output);
	}

	// erosion of the dilation, remove black holes
	template <class T>
    void Morphology<T>::close(const cv::Mat_<T> & input, cv::Mat_<T> & output) const
	{
		dilate(input, output);
		erode(output, output);
	}

	// dilation - erosion
	template <class T>
    void Morphology<T>::morphGradient(const cv::Mat_<T> & input, cv::Mat_<T> & output) const
	{
        cv::Mat_<T> dilated;
        cv::Mat_<T> eroded;
		erode(input, eroded);
		dilate(input, dilated);

		output = dilated - eroded;
	}

	// return objects that are smaller than the structuring element
	// where the structuring element does not fit in
	// and brighter than their surroundings
	template <class T>
    void Morphology<T>::topHatWhite(const cv::Mat_<T> & input, cv::Mat_<T> & output) const
	{
        cv::Mat_<T> opened;
		open(input, opened);

		output = input - opened;
	}

	// return objects that are smaller than the structuring element
	// where the structuring element does not fit in
	// and darker than their surroundings
	template <class T>
    void Morphology<T>::topHatBlack(const cv::Mat_<T> & input, cv::Mat_<T> & output) const
	{
        cv::Mat_<T> closed;
		close(input, closed);

		output = closed - input;
	}

} // namespace my


#endif // MORPHOLOGY_H

