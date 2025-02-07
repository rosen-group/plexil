<?xml version="1.0" encoding="utf-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
  <Node NodeType="NodeList" epx="CheckedSequence">
    <NodeId generated="1">ep2cp_CheckedSequence_d14e3</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>foo</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <InvariantCondition>
      <NoChildFailed>
        <NodeRef dir="self"/>
      </NoChildFailed>
    </InvariantCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty">
          <NodeId>One</NodeId>
        </Node>
        <Node NodeType="NodeList" epx="CheckedSequence">
          <NodeId generated="1">ep2cp_CheckedSequence_d14e25</NodeId>
          <StartCondition>
            <AND>
              <Finished>
                <NodeRef dir="sibling">One</NodeRef>
              </Finished>
              <EQNumeric>
                <IntegerVariable>foo</IntegerVariable>
                <IntegerValue>0</IntegerValue>
              </EQNumeric>
            </AND>
          </StartCondition>
          <InvariantCondition>
            <NoChildFailed>
              <NodeRef dir="self"/>
            </NoChildFailed>
          </InvariantCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="Empty">
                <NodeId>Two</NodeId>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
        <Node NodeType="Empty">
          <NodeId>Three</NodeId>
          <StartCondition>
            <Finished>
              <NodeRef dir="sibling">ep2cp_CheckedSequence_d14e25</NodeRef>
            </Finished>
          </StartCondition>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
