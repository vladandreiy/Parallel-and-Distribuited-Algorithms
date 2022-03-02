package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	ArrayBlockingQueue<Integer> arr = new ArrayBlockingQueue<>(5);

	void put(int value) {
		try {
			arr.put(value);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	int get() {
		int val = -1;
		try {
			val = arr.take();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		return val;
	}
}
