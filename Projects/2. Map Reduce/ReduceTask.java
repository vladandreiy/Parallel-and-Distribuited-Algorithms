import java.util.List;

public class ReduceTask {
    String documentName;
    List<MapResult> mapResults;

    public ReduceTask(String documentName, List<MapResult> mapResults) {
        this.documentName = documentName;
        this.mapResults = mapResults;
    }

    public String getDocumentName() {
        return documentName;
    }
}
