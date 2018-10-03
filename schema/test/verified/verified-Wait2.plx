<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="Sequence">
    <NodeId>ep2cp_Sequence_d1e3</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>x</Name>
        <Type>Real</Type>
        <InitialValue>
          <RealValue>100.3</RealValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <InvariantCondition>
      <NOT>
        <OR>
          <AND>
            <Finished>
              <NodeRef dir="child">Wait1</NodeRef>
            </Finished>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">Wait1</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
          </AND>
          <AND>
            <Finished>
              <NodeRef dir="child">Wait2</NodeRef>
            </Finished>
            <EQInternal>
              <NodeOutcomeVariable>
                <NodeRef dir="child">Wait2</NodeRef>
              </NodeOutcomeVariable>
              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
            </EQInternal>
          </AND>
        </OR>
      </NOT>
    </InvariantCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty" epx="Wait" FileName="foo.ple" LineNo="104" ColNo="1">
          <NodeId>Wait1</NodeId>
          <EndCondition>
            <GE>
              <LookupOnChange>
                <Name>
                  <StringValue>time</StringValue>
                </Name>
                <Tolerance>
                  <RealValue>0.2</RealValue>
                </Tolerance>
              </LookupOnChange>
              <ADD>
                <RealValue>23.9</RealValue>
                <NodeTimepointValue>
                  <NodeId>Wait1</NodeId>
                  <NodeStateValue>EXECUTING</NodeStateValue>
                  <Timepoint>START</Timepoint>
                </NodeTimepointValue>
              </ADD>
            </GE>
          </EndCondition>
        </Node>
        <Node NodeType="Empty" epx="Wait" FileName="foo.ple" LineNo="108" ColNo="1">
          <NodeId>Wait2</NodeId>
          <StartCondition>
            <Finished>
              <NodeRef dir="sibling">Wait1</NodeRef>
            </Finished>
          </StartCondition>
          <EndCondition>
            <GE>
              <LookupOnChange>
                <Name>
                  <StringValue>time</StringValue>
                </Name>
                <Tolerance>
                  <RealVariable>x</RealVariable>
                </Tolerance>
              </LookupOnChange>
              <ADD>
                <RealVariable>x</RealVariable>
                <NodeTimepointValue>
                  <NodeId>Wait2</NodeId>
                  <NodeStateValue>EXECUTING</NodeStateValue>
                  <Timepoint>START</Timepoint>
                </NodeTimepointValue>
              </ADD>
            </GE>
          </EndCondition>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
