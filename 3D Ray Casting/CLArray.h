#include "cl.h"

template <class T>
class CLArray {
private:
	T* m_array;
	cl::Buffer m_buffer;

	int m_size, m_elemSize, m_dataSize;

public:
	CLArray() {}

	CLArray(int size, cl::Context &context, int flags) {
		init(size, context, flags);
	}

	void init(int size, cl::Context &context, int flags) {
		m_elemSize = sizeof(T);

		m_size = size;
		m_dataSize = m_elemSize * m_size;

		m_array = new T[size];
		m_buffer = cl::Buffer(context, flags, m_dataSize);
	}

	void release(void) {
		delete[] m_array;
	}

	void writeBuffer(cl::CommandQueue &queue) {
		queue.enqueueWriteBuffer(m_buffer, CL_TRUE, 0, m_dataSize, &(m_array[0]));
	}

	void readBuffer(cl::CommandQueue &queue) {
		queue.enqueueReadBuffer(m_buffer, CL_TRUE, 0, m_dataSize, &(m_array[0]));
	}

	void setAsArg(cl::Kernel &kernel, int i) {
		kernel.setArg(i, m_buffer);
	}

	T& operator[](unsigned int i) { return m_array[i]; }
	const T& operator[](unsigned int i) const { return m_array[i]; }

	int size(void) {
		return m_size;
	}

	int dataSize(void) {
		return m_dataSize;
	}
};