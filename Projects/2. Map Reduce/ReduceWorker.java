import java.util.Map;
import java.util.concurrent.Callable;

public class ReduceWorker implements Callable<ReduceResult> {
    private final ReduceTask reduceTask;

    public ReduceWorker(ReduceTask reduceTask) {
        this.reduceTask = reduceTask;
    }

    @Override
    public ReduceResult call() throws Exception {
        return resolveReduceTask();
    }

    public ReduceResult resolveReduceTask() {
        float rank = 0;
        int longestWordSize = 0;
        int longestWordsNo = 0;
        int wordsNo = 0;
        for (MapResult mapResult : reduceTask.mapResults) {
            for (Map.Entry<Integer, Integer> entry : mapResult.dictionary.entrySet()) {
                if (entry.getKey() == 0)
                    continue;
                // Add to rank the value of Fibonacci(wordSize * appearances)
                rank += Fibonacci(entry.getKey() + 1) * entry.getValue();
                // Increase the number of words
                wordsNo = wordsNo + entry.getValue();
                if (entry.getKey() > longestWordSize) {
                    // If current word is longer than last longestWord, set longestWordSize and longestWordsNo
                    longestWordSize = entry.getKey();
                    longestWordsNo = entry.getValue();
                } else if (entry.getKey() == longestWordSize)
                    // If current word is as long as the longestWord, increase longestWordsNo
                    longestWordsNo += entry.getValue();
            }
        }
        // Divide rank by number of words
        rank /= wordsNo;
        return new ReduceResult(reduceTask.getDocumentName(), rank, longestWordSize, longestWordsNo);
    }

    public static int Fibonacci(int n) {
        int a = 0, b = 1, c, i;
        if (n == 0)
            return a;
        for (i = 2; i <= n; i++) {
            c = a + b;
            a = b;
            b = c;
        }
        return b;
    }
}
