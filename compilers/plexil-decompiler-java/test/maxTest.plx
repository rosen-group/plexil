<?xml version="1.0" encoding="UTF-8"?><!-- Generated by PlexiLisp --><PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator"><Node NodeType="NodeList"><NodeId>maxTest</NodeId><VariableDeclarations>
      <DeclareVariable>
        <Name>l</Name>
        <Type>Integer</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>m</Name>
        <Type>Integer</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>n</Name>
        <Type>Integer</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>x</Name>
        <Type>Real</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>y</Name>
        <Type>Real</Type>
      </DeclareVariable>
    </VariableDeclarations><PostCondition><AND><NOT><IsKnown><IntegerVariable>l</IntegerVariable></IsKnown></NOT><EQNumeric><IntegerVariable>m</IntegerVariable><IntegerValue>1</IntegerValue></EQNumeric><EQNumeric><IntegerVariable>n</IntegerVariable><IntegerValue>2</IntegerValue></EQNumeric><EQNumeric><RealVariable>x</RealVariable><RealValue>2.0</RealValue></EQNumeric><EQNumeric><RealVariable>y</RealVariable><RealValue>7.1</RealValue></EQNumeric></AND></PostCondition><NodeBody>
      <NodeList>
        <Node NodeType="Assignment"><NodeId>unknownInts</NodeId><NodeBody>
            <Assignment>
              <IntegerVariable>l</IntegerVariable>
              <NumericRHS>
                <MAX>
                  <IntegerVariable>l</IntegerVariable>
                  <IntegerValue>0</IntegerValue>
                </MAX>
              </NumericRHS>
            </Assignment>
          </NodeBody></Node>
        <Node NodeType="Assignment"><NodeId>equalInts</NodeId><NodeBody>
            <Assignment>
              <IntegerVariable>m</IntegerVariable>
              <NumericRHS>
                <MAX>
                  <IntegerValue>1</IntegerValue>
                  <IntegerValue>1</IntegerValue>
                </MAX>
              </NumericRHS>
            </Assignment>
          </NodeBody></Node>
        <Node NodeType="Assignment"><NodeId>unequalInts</NodeId><NodeBody>
            <Assignment>
              <IntegerVariable>n</IntegerVariable>
              <NumericRHS>
                <MAX>
                  <IntegerValue>1</IntegerValue>
                  <IntegerValue>2</IntegerValue>
                </MAX>
              </NumericRHS>
            </Assignment>
          </NodeBody></Node>
        <Node NodeType="Assignment"><NodeId>equalReals</NodeId><NodeBody>
            <Assignment>
              <RealVariable>x</RealVariable>
              <NumericRHS>
                <MAX>
                  <RealValue>2.0</RealValue>
                  <RealValue>2.0</RealValue>
                </MAX>
              </NumericRHS>
            </Assignment>
          </NodeBody></Node>
        <Node NodeType="Assignment"><NodeId>unequal</NodeId><NodeBody>
            <Assignment>
              <RealVariable>y</RealVariable>
              <NumericRHS>
                <MAX>
                  <RealValue>7.0</RealValue>
                  <RealValue>7.1</RealValue>
                </MAX>
              </NumericRHS>
            </Assignment>
          </NodeBody></Node>
      </NodeList>
    </NodeBody></Node></PlexilPlan>