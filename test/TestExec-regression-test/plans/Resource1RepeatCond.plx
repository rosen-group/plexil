<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan>
  <GlobalDeclarations>
    <CommandDeclaration>
      <Name>pickup_object</Name>
      <Return>
        <Type>Integer</Type>
      </Return>
      <Parameter>
        <Type>Integer</Type>
      </Parameter>
    </CommandDeclaration>
    <CommandDeclaration>
      <Name>process_image</Name>
      <Return>
        <Type>Integer</Type>
      </Return>
    </CommandDeclaration>
    <CommandDeclaration>
      <Name>take_picture</Name>
      <Return>
        <Type>Integer</Type>
      </Return>
    </CommandDeclaration>
  </GlobalDeclarations>
  <Node NodeType="NodeList">
    <NodeId>Resource1RepeatCond</NodeId>
    <NodeBody>
      <NodeList>
        <Node NodeType="Command">
          <NodeId>PickupObject</NodeId>
          <Priority>10</Priority>
          <EndCondition>
            <EQNumeric>
              <IntegerValue>10</IntegerValue>
              <IntegerVariable>returnValue</IntegerVariable>
            </EQNumeric>
          </EndCondition>
          <PostCondition>
            <EQInternal>
              <NodeCommandHandleVariable>
                <NodeId>PickupObject</NodeId>
              </NodeCommandHandleVariable>
              <NodeCommandHandleValue>COMMAND_SENT_TO_SYSTEM</NodeCommandHandleValue>
            </EQInternal>
          </PostCondition>
          <VariableDeclarations>
            <DeclareVariable>
              <Name>returnValue</Name>
              <Type>Integer</Type>
              <InitialValue>
                <IntegerValue>-1</IntegerValue>
              </InitialValue>
            </DeclareVariable>
          </VariableDeclarations>
          <NodeBody>
            <Command>
              <ResourceList>
                <Resource>
                  <ResourceName>
                    <StringValue>right_arm</StringValue>
                  </ResourceName>
                </Resource>
                <Resource>
                  <ResourceName>
                    <StringValue>sys_memory</StringValue>
                  </ResourceName>
                  <ResourceUpperBound>
                    <RealValue>1.0</RealValue>
                  </ResourceUpperBound>
                </Resource>
              </ResourceList>
              <IntegerVariable>returnValue</IntegerVariable>
              <Name><StringValue>pickup_object</StringValue></Name>
              <Arguments>
                <IntegerValue>20</IntegerValue>
              </Arguments>
            </Command>
          </NodeBody>
        </Node>
        <Node NodeType="Command">
          <NodeId>ProcessImage</NodeId>
          <Priority>30</Priority>
          <RepeatCondition>
            <EQInternal>
              <NodeCommandHandleVariable>
                <NodeId>ProcessImage</NodeId>
              </NodeCommandHandleVariable>
              <NodeCommandHandleValue>COMMAND_DENIED</NodeCommandHandleValue>
            </EQInternal>
          </RepeatCondition>
          <PostCondition>
            <EQInternal>
              <NodeCommandHandleVariable>
                <NodeId>ProcessImage</NodeId>
              </NodeCommandHandleVariable>
              <NodeCommandHandleValue>COMMAND_SENT_TO_SYSTEM</NodeCommandHandleValue>
            </EQInternal>
          </PostCondition>
          <NodeBody>
            <Command>
              <ResourceList>
                <Resource>
                  <ResourceName>
                    <StringValue>sys_memory</StringValue>
                  </ResourceName>
                  <ResourceUpperBound>
                    <RealValue>1.0</RealValue>
                  </ResourceUpperBound>
                </Resource>
              </ResourceList>
              <Name><StringValue>process_image</StringValue></Name>
            </Command>
          </NodeBody>
        </Node>
        <Node NodeType="Command">
          <NodeId>TakePicture</NodeId>
          <Priority>10</Priority>
          <PostCondition>
            <EQInternal>
              <NodeCommandHandleVariable>
                <NodeId>TakePicture</NodeId>
              </NodeCommandHandleVariable>
              <NodeCommandHandleValue>COMMAND_SENT_TO_SYSTEM</NodeCommandHandleValue>
            </EQInternal>
          </PostCondition>
          <NodeBody>
            <Command>
              <ResourceList>
                <Resource>
                  <ResourceName>
                    <StringValue>vision_system</StringValue>
                  </ResourceName>
                </Resource>
              </ResourceList>
              <Name><StringValue>take_picture</StringValue></Name>
            </Command>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
