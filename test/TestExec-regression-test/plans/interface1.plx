<?xml version="1.0" encoding="UTF-8"?><!-- Generated by PlexiLisp --><PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator"><Node NodeType="NodeList"><NodeId>interface1</NodeId><VariableDeclarations>
      <DeclareVariable>
        <Name>foo</Name>
        <Type>Integer</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>bar</Name>
        <Type>Integer</Type>
      </DeclareVariable>
    </VariableDeclarations><NodeBody>
      <NodeList>
        <Node NodeType="Assignment"><NodeId>One</NodeId><NodeBody>
            <Assignment>
              <IntegerVariable>bar</IntegerVariable>
              <NumericRHS>
                <IntegerValue>55</IntegerValue>
              </NumericRHS>
            </Assignment>
          </NodeBody></Node>
        <Node NodeType="Assignment"><NodeId>Two</NodeId><Interface><In><DeclareVariable>
                <Name>bar</Name>
                <Type>Integer</Type>
              </DeclareVariable></In></Interface><NodeBody>
            <Assignment>
              <IntegerVariable>foo</IntegerVariable>
              <NumericRHS>
                <IntegerVariable>bar</IntegerVariable>
              </NumericRHS>
            </Assignment>
          </NodeBody></Node>
      </NodeList>
    </NodeBody></Node></PlexilPlan>