package task6;


import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.RecursiveTask;

public class Task extends RecursiveTask<Void> {
	private final int[] graph;
	private final int step;

	public Task(int[] graph, int step) {
		this.graph = graph;
		this.step = step;
	}

	@Override
	protected Void compute() {

		if (Main.N == step) {
			Main.printQueens(graph);
			return null;
		}

		List<Task> tasks = new ArrayList<>();
		for (int i = 0; i < Main.N; ++i) {
			int[] newGraph = graph.clone();
			newGraph[step] = i;

			if (Main.check(newGraph, step)) {
				Task t = new Task(newGraph, step + 1);
				tasks.add(t);
				t.fork();
			}
		}

		for (var task: tasks) {
			task.join();
		}
		return null;
	}
}
