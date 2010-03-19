<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator"
            xmlns:exslt="http://exslt.org/common">
   <Node NodeType="NodeList">
      <NodeId>Root</NodeId>
      <VariableDeclarations>
         <DeclareVariable>
            <Name>foo</Name>
            <Type>Integer</Type>
            <InitialValue>
               <IntegerValue>0</IntegerValue>
            </InitialValue>
         </DeclareVariable>
      </VariableDeclarations>
      <NodeBody>
         <NodeList>
            <Node NodeType="Assignment">
               <NodeId>One</NodeId>
               <PostCondition>
                  <EQNumeric>
                     <IntegerVariable>foo</IntegerVariable>
                     <IntegerValue>3</IntegerValue>
                  </EQNumeric>
               </PostCondition>
               <NodeBody>
                  <Assignment>
                     <IntegerVariable>foo</IntegerVariable>
                     <NumericRHS>
                        <IntegerValue>2</IntegerValue>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>

            <Node NodeType="Assignment">
               <NodeId>Two</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>One</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                     <EQInternal>
                        <NodeFailureVariable>
                           <NodeId>One</NodeId>
                        </NodeFailureVariable>
                        <NodeFailureValue>POST_CONDITION_FAILED</NodeFailureValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
               <PostCondition>
                  <EQNumeric>
                     <IntegerVariable>foo</IntegerVariable>
                     <IntegerValue>6</IntegerValue>
                  </EQNumeric>
               </PostCondition>
               <NodeBody>
                  <Assignment>
                     <IntegerVariable>foo</IntegerVariable>
                     <NumericRHS>
                        <IntegerValue>6</IntegerValue>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>

            <Node NodeType="Assignment">
               <NodeId>Three</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeId>Two</NodeId>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <NodeBody>
                  <Assignment>
                     <IntegerVariable>foo</IntegerVariable>
                     <NumericRHS>
                        <IntegerValue>2</IntegerValue>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>

         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>