<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator"
            FileName="SimpleAssignment.ple">
   <Node NodeType="Assignment" LineNo="6" ColNo="2">
      <NodeId>SimpleAssignment</NodeId>
      <VariableDeclarations>
         <DeclareVariable LineNo="3" ColNo="2">
            <Name>foo</Name>
            <Type>Integer</Type>
            <InitialValue>
               <IntegerValue>0</IntegerValue>
            </InitialValue>
         </DeclareVariable>
      </VariableDeclarations>
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
               <IntegerValue>3</IntegerValue>
            </NumericRHS>
         </Assignment>
      </NodeBody>
   </Node>
</PlexilPlan>