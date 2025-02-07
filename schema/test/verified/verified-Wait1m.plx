<?xml version="1.0" encoding="utf-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
   <GlobalDeclarations>
      <DeclareMutex>
         <Name>m</Name>
      </DeclareMutex>
   </GlobalDeclarations>
   <Node NodeType="Empty"
          epx="Wait"
          FileName="foo.ple"
          LineNo="104"
          ColNo="1">
      <NodeId>Wait1</NodeId>
      <UsingMutex>
         <Name>m</Name>
      </UsingMutex>
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
                  <NodeRef dir="self"/>
                  <NodeStateValue>EXECUTING</NodeStateValue>
                  <Timepoint>START</Timepoint>
               </NodeTimepointValue>
            </ADD>
         </GE>
      </EndCondition>
   </Node>
</PlexilPlan>
