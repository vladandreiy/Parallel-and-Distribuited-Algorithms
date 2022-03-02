public class MapTask {
    String documentName;
    int offset;
    int fragmentDim;

    public MapTask(String documentName, int offset, int fragmentDim) {
        this.documentName = documentName;
        this.offset = offset;
        this.fragmentDim = fragmentDim;
    }

    public String getDocumentName() {
        return documentName;
    }

    public int getOffset() {
        return offset;
    }

    public int getFragmentDim() {
        return fragmentDim;
    }
}
