<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="UncheckedSequence">
      <NodeId>UncheckedSequence_d1e3</NodeId>
      <NodeBody>
         <NodeList>
            <Node NodeType="Empty">
               <NodeId>One</NodeId>
            </Node>
            <Node NodeType="Empty">
               <NodeId>Two</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>One</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>