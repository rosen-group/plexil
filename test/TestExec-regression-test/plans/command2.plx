<?xml version="1.0" encoding="UTF-8"?><PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" FileName="plans/command2.ple"><Node NodeType="Command" ColNo="0" LineNo="1"><NodeId>command2</NodeId><VariableDeclarations><DeclareVariable ColNo="2" LineNo="3"><Name>CommandName</Name><Type>String</Type><InitialValue><StringValue>foo</StringValue></InitialValue></DeclareVariable></VariableDeclarations><NodeBody><Command ColNo="3" LineNo="5"><Name><LookupNow><Name><Concat ColNo="20" LineNo="5"><StringValue>bar</StringValue><StringVariable>CommandName</StringVariable></Concat></Name></LookupNow></Name></Command></NodeBody></Node></PlexilPlan>