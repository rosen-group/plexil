<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	    xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <Node NodeType="NodeList">
    <NodeId>Grandparent</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>x</Name>
        <Type>Boolean</Type>
        <InitialValue>
          <BooleanValue>0</BooleanValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <EndCondition>
      <EQInternal>
	<NodeStateVariable>
	  <NodeId>Parent</NodeId>
	</NodeStateVariable>
	<NodeStateValue>FINISHED</NodeStateValue>
      </EQInternal>
    </EndCondition>
    <PostCondition>
      <EQInternal>
	<NodeOutcomeVariable>
	  <NodeId>Parent</NodeId>
	</NodeOutcomeVariable>
	<NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
      </EQInternal>
    </PostCondition>
    <NodeBody>
      <NodeList>
	<Node NodeType="NodeList">
	  <NodeId>Parent</NodeId>
	  <Interface>
	    <In>
<!--               <BooleanVariable>x</BooleanVariable> -->
              <DeclareVariable>
                <Name>x</Name>
                <Type>Boolean</Type>
                <InitialValue>
                  <BooleanValue>0</BooleanValue>
                </InitialValue>
              </DeclareVariable>
	    </In>
	  </Interface>
	  <VariableDeclarations>
      <DeclareVariable>
        <Name>y</Name>
        <Type>Boolean</Type>
        <InitialValue>
          <BooleanValue>1</BooleanValue>
        </InitialValue>
      </DeclareVariable>
	  </VariableDeclarations>
	  <EndCondition>
	    <EQInternal>
	      <NodeStateVariable>
		<NodeId>Child</NodeId>
	      </NodeStateVariable>
	      <NodeStateValue>FINISHED</NodeStateValue>
	    </EQInternal>
	  </EndCondition>
          <PostCondition>
            <EQBoolean>
              <BooleanVariable>y</BooleanVariable>
              <BooleanValue>0</BooleanValue>
            </EQBoolean>
          </PostCondition>
	  <NodeBody>
	    <NodeList>
	      <Node NodeType="Assignment">
		<NodeId>Child</NodeId>
		<Interface>
		  <InOut>
<!--                     <BooleanVariable>y</BooleanVariable> -->
                    <DeclareVariable>
                      <Name>y</Name>
                      <Type>Boolean</Type>
                      <InitialValue>
                        <BooleanValue>0</BooleanValue>
                      </InitialValue>
                    </DeclareVariable>
		  </InOut>
		  <In>
<!-- 		    <BooleanVariable>x</BooleanVariable> -->
                    <DeclareVariable>
                      <Name>x</Name>
                      <Type>Boolean</Type>
                      <InitialValue>
                        <BooleanValue>0</BooleanValue>
                      </InitialValue>
                    </DeclareVariable>
		  </In>
		</Interface>
		<NodeBody>
		  <Assignment>
		    <BooleanVariable>y</BooleanVariable>
		    <BooleanRHS>
		      <BooleanVariable>x</BooleanVariable>
		    </BooleanRHS>
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
