package org.deeplearning4j.graph.api;

/**
 * Created by Alex on 9/11/2015.
 */
public interface VertexFactory<T> {

    public Vertex<T> create(int vertexIdx);

}
