<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tr="extended-plexil-translator">
  <Node NodeType="NodeList" epx="If">
    <NodeId>ep2cp_If_d1e3</NodeId>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty" epx="Then">
          <StartCondition>
            <BooleanValue>true</BooleanValue>
          </StartCondition>
          <SkipCondition>
            <NOT>
              <BooleanValue>true</BooleanValue>
            </NOT>
          </SkipCondition>
          <NodeId>One</NodeId>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
