<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated by PlexiLisp -->
<PlexilPlan>
  <Node NodeType="NodeList" >
    <NodeId>cruise-control</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>cruise-on</Name>
        <Type>Boolean</Type>
        <InitialValue>
          <BooleanValue>false</BooleanValue>
        </InitialValue>
      </DeclareVariable>
      <DeclareVariable>
        <Name>target-speed</Name>
        <Type>Real</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>throttle-pos</Name>
        <Type>Real</Type>
        <InitialValue>
          <RealValue>0.0</RealValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" >
          <NodeId>SET</NodeId>
          <StartCondition>
            <LookupOnChange>
              <Name>
                <StringValue>SetPressed</StringValue>
              </Name>
            </LookupOnChange>
          </StartCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="Assignment" >
                <NodeId>set-cruise-on</NodeId>
                <NodeBody>
                  <Assignment>
                    <BooleanVariable>cruise-on</BooleanVariable>
                    <BooleanRHS>
                      <BooleanValue>1</BooleanValue>
                    </BooleanRHS>
                  </Assignment>
                </NodeBody>
              </Node>
              <Node NodeType="Assignment" >
                <NodeId>set-target-speed</NodeId>
                <NodeBody>
                  <Assignment>
                    <RealVariable>target-speed</RealVariable>
                    <NumericRHS>
                      <LookupNow>
                        <Name>
                          <StringValue>actual-speed</StringValue>
                        </Name>
                      </LookupNow>
                    </NumericRHS>
                  </Assignment>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
        <Node NodeType="Assignment" >
          <NodeId>CANCEL</NodeId>
          <StartCondition>
            <LookupOnChange>
              <Name>
                <StringValue>CancelPressed</StringValue>
              </Name>
            </LookupOnChange>
          </StartCondition>
          <RepeatCondition>
            <BooleanValue>1</BooleanValue>
          </RepeatCondition>
          <NodeBody>
            <Assignment>
              <BooleanVariable>cruise-on</BooleanVariable>
              <BooleanRHS>
                <BooleanValue>0</BooleanValue>
              </BooleanRHS>
            </Assignment>
          </NodeBody>
        </Node>
        <Node NodeType="Empty" >
          <NodeId>RESUME</NodeId>
        </Node>
        <Node NodeType="Empty" >
          <NodeId>ACCEL</NodeId>
        </Node>
        <Node NodeType="Empty" >
          <NodeId>DECEL</NodeId>
        </Node>
        <Node NodeType="Empty" >
          <NodeId>RESUME</NodeId>
        </Node>
        <Node NodeType="NodeList" >
          <NodeId>control-speed-automatically</NodeId>
          <StartCondition>
            <BooleanVariable>cruise-on</BooleanVariable>
          </StartCondition>
          <EndCondition>
            <NOT>
              <BooleanVariable>cruise-on</BooleanVariable>
            </NOT>
          </EndCondition>
          <RepeatCondition>
            <BooleanVariable>cruise-on</BooleanVariable>
          </RepeatCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="Assignment" >
                <NodeId>increase-throttle</NodeId>
                <StartCondition>
                  <LT>
                    <LookupOnChange>
                      <Name>
                        <StringValue>actual-speed</StringValue>
                      </Name>
                    </LookupOnChange>
                    <RealVariable>target-speed</RealVariable>
                  </LT>
                </StartCondition>
                <NodeBody>
                  <Assignment>
                    <RealVariable>throttle-pos</RealVariable>
                    <NumericRHS>
                      <ADD>
                        <RealVariable>throttle-pos</RealVariable>
                        <RealValue>0.1</RealValue>
                      </ADD>
                    </NumericRHS>
                  </Assignment>
                </NodeBody>
              </Node>
              <Node NodeType="Assignment" >
                <NodeId>decrease-throttle</NodeId>
                <StartCondition>
                  <GT>
                    <LookupOnChange>
                      <Name>
                        <StringValue>actual-speed</StringValue>
                      </Name>
                    </LookupOnChange>
                    <RealVariable>target-speed</RealVariable>
                  </GT>
                </StartCondition>
                <NodeBody>
                  <Assignment>
                    <RealVariable>throttle-pos</RealVariable>
                    <NumericRHS>
                      <SUB>
                        <RealVariable>throttle-pos</RealVariable>
                        <RealValue>0.1</RealValue>
                      </SUB>
                    </NumericRHS>
                  </Assignment>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
