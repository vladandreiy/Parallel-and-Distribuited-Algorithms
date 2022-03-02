import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.concurrent.*;

public class Tema2 {
    public static void main(String[] args) throws IOException, InterruptedException, ExecutionException {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }
        ExecutorService executorService = Executors.newFixedThreadPool(Integer.parseInt(args[0]));
        Scanner scanner = new Scanner(new File(args[1]));
        int fragmentDim = scanner.nextInt();
        int documentsNo = scanner.nextInt();
        List<String> documentNames = new ArrayList<>();
        for (int i = 0; i < documentsNo; i++)
            documentNames.add(scanner.next());

        // Create map tasks
        List<MapWorker> mapWorkers = createMapTasks(fragmentDim, documentNames);

        // Add tasks to pool of map tasks
        List<Future<MapResult>> mapFutures = executorService.invokeAll(mapWorkers);

        // Get results of map tasks
        List<MapResult> mapResults = new ArrayList<>();
        for (Future<MapResult> future : mapFutures) {
            mapResults.add(future.get());
        }

        // Create reduce tasks
        List<ReduceWorker> reduceWorkers = createReduceTasks(mapResults);

        // Add tasks to pool of reduce tasks
        List<Future<ReduceResult>> reduceFutures = executorService.invokeAll(reduceWorkers);

        // Get results of reduce tasks
        List<ReduceResult> reduceResults = new ArrayList<>();
        for (Future<ReduceResult> future : reduceFutures) {
            reduceResults.add(future.get());
        }
        executorService.shutdown();

        // Sort reduce results by rank
        reduceResults.sort(Comparator.comparingDouble(ReduceResult::getRank).reversed());

        // Write output to file
        Writer writer = new BufferedWriter(new OutputStreamWriter(
                new FileOutputStream(args[2]), StandardCharsets.UTF_8));
        for (ReduceResult result : reduceResults) {
            writer.write(result.toString() + "\n");
        }
        writer.close();
    }

    static List<MapWorker> createMapTasks(int fragmentDim, List<String> documentNames) throws IOException {
        long charCount;
        List<MapWorker> mapWorkers = new ArrayList<>();
        for (String documentName : documentNames) {
            // Get number of characters in file
            charCount = Files.size(Paths.get(documentName));
            // Increase offset by fragmentDimension, as long as offset is smaller than the number of characters in the file
            for (int i = 0, offset = 0; offset < charCount; i++, offset = i * fragmentDim)
                mapWorkers.add(new MapWorker(new MapTask(documentName, offset, fragmentDim)));
        }
        return mapWorkers;
    }

    static List<ReduceWorker> createReduceTasks(List<MapResult> mapResults) {
        // Store tasks in map with the key being the documentName
        Map<String, List<MapResult>> reduceTasksMap = new LinkedHashMap<>();
        for (MapResult mapResult : mapResults) {
            if (!reduceTasksMap.containsKey(mapResult.getDocumentName()))
                reduceTasksMap.put(mapResult.getDocumentName(), new ArrayList<>(List.of(mapResult)));
            else
                reduceTasksMap.get(mapResult.getDocumentName()).add(mapResult);
        }
        List<ReduceWorker> reduceWorkers = new ArrayList<>();
        for (Map.Entry<String, List<MapResult>> entry : reduceTasksMap.entrySet())
            reduceWorkers.add(new ReduceWorker(new ReduceTask(entry.getKey(), entry.getValue())));
        return reduceWorkers;
    }
}

