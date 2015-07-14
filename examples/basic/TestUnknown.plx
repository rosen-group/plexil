<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator"
            FileName="TestUnknown.ple">
   <GlobalDeclarations LineNo="2" ColNo="7">
      <CommandDeclaration LineNo="2" ColNo="7">
         <Name>get_string</Name>
         <Return>
            <Name>_return_0</Name>
            <Type>String</Type>
         </Return>
      </CommandDeclaration>
      <CommandDeclaration LineNo="3" ColNo="8">
         <Name>get_int</Name>
         <Return>
            <Name>_return_0</Name>
            <Type>Integer</Type>
         </Return>
      </CommandDeclaration>
      <CommandDeclaration LineNo="4" ColNo="5">
         <Name>get_real</Name>
         <Return>
            <Name>_return_0</Name>
            <Type>Real</Type>
         </Return>
      </CommandDeclaration>
      <CommandDeclaration LineNo="5" ColNo="8">
         <Name>get_bool</Name>
         <Return>
            <Name>_return_0</Name>
            <Type>Boolean</Type>
         </Return>
      </CommandDeclaration>
      <CommandDeclaration LineNo="6" ColNo="0">
         <Name>pprint</Name>
      </CommandDeclaration>
   </GlobalDeclarations>
   <Node NodeType="NodeList" epx="Sequence" LineNo="10" ColNo="2">
      <NodeId>TestUnknown</NodeId>
      <VariableDeclarations>
         <DeclareVariable LineNo="9" ColNo="2">
            <Name>r</Name>
            <Type>Real</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="9" ColNo="2">
            <Name>r1</Name>
            <Type>Real</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="9" ColNo="2">
            <Name>r2</Name>
            <Type>Real</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="10" ColNo="2">
            <Name>i</Name>
            <Type>Integer</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="10" ColNo="2">
            <Name>i1</Name>
            <Type>Integer</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="10" ColNo="2">
            <Name>i2</Name>
            <Type>Integer</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="11" ColNo="2">
            <Name>s</Name>
            <Type>String</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="11" ColNo="2">
            <Name>s1</Name>
            <Type>String</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="11" ColNo="2">
            <Name>s2</Name>
            <Type>String</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="12" ColNo="2">
            <Name>b</Name>
            <Type>Boolean</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="12" ColNo="2">
            <Name>b1</Name>
            <Type>Boolean</Type>
         </DeclareVariable>
         <DeclareVariable LineNo="12" ColNo="2">
            <Name>b2</Name>
            <Type>Boolean</Type>
         </DeclareVariable>
      </VariableDeclarations>
      <InvariantCondition>
         <NOT>
            <OR>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestVars1</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestVars1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestString1</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestString1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestString2</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestString2</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestInt1</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestInt1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestInt2</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestInt2</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestReal1</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestReal1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestReal2</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestReal2</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestBool1</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestBool1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
               <AND>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeRef dir="child">TestBool2</NodeRef>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="child">TestBool2</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </AND>
            </OR>
         </NOT>
      </InvariantCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="NodeList" epx="Concurrence" LineNo="15" ColNo="13">
               <NodeId>TestVars1</NodeId>
               <PostCondition>
                  <AND>
                     <IsKnown>
                        <RealVariable>r1</RealVariable>
                     </IsKnown>
                     <EQNumeric>
                        <RealVariable>r1</RealVariable>
                        <RealValue>0.0</RealValue>
                     </EQNumeric>
                     <IsKnown>
                        <IntegerVariable>i1</IntegerVariable>
                     </IsKnown>
                     <EQNumeric>
                        <IntegerVariable>i1</IntegerVariable>
                        <IntegerValue>0</IntegerValue>
                     </EQNumeric>
                     <IsKnown>
                        <StringVariable>s1</StringVariable>
                     </IsKnown>
                     <EQString>
                        <StringVariable>s1</StringVariable>
                        <StringValue>foo</StringValue>
                     </EQString>
                     <IsKnown>
                        <BooleanVariable>b1</BooleanVariable>
                     </IsKnown>
                     <BooleanVariable>b1</BooleanVariable>
                     <NOT>
                        <IsKnown>
                           <RealVariable>r2</RealVariable>
                        </IsKnown>
                     </NOT>
                     <NOT>
                        <IsKnown>
                           <IntegerVariable>i2</IntegerVariable>
                        </IsKnown>
                     </NOT>
                     <NOT>
                        <IsKnown>
                           <StringVariable>s2</StringVariable>
                        </IsKnown>
                     </NOT>
                     <NOT>
                        <IsKnown>
                           <BooleanVariable>b2</BooleanVariable>
                        </IsKnown>
                     </NOT>
                  </AND>
               </PostCondition>
               <NodeBody>
                  <NodeList>
                     <Node NodeType="Assignment" LineNo="21" ColNo="4">
                        <NodeId>ASSIGNMENT__0</NodeId>
                        <NodeBody>
                           <Assignment>
                              <RealVariable>r1</RealVariable>
                              <NumericRHS>
                                 <LookupNow>
                                    <Name>
                                       <StringValue>r1</StringValue>
                                    </Name>
                                 </LookupNow>
                              </NumericRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" LineNo="23" ColNo="4">
                        <NodeId>ASSIGNMENT__1</NodeId>
                        <NodeBody>
                           <Assignment>
                              <RealVariable>r2</RealVariable>
                              <NumericRHS>
                                 <LookupNow>
                                    <Name>
                                       <StringValue>r2</StringValue>
                                    </Name>
                                 </LookupNow>
                              </NumericRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" LineNo="25" ColNo="4">
                        <NodeId>ASSIGNMENT__2</NodeId>
                        <NodeBody>
                           <Assignment>
                              <IntegerVariable>i1</IntegerVariable>
                              <NumericRHS>
                                 <LookupNow>
                                    <Name>
                                       <StringValue>i1</StringValue>
                                    </Name>
                                 </LookupNow>
                              </NumericRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" LineNo="27" ColNo="4">
                        <NodeId>ASSIGNMENT__3</NodeId>
                        <NodeBody>
                           <Assignment>
                              <IntegerVariable>i2</IntegerVariable>
                              <NumericRHS>
                                 <LookupNow>
                                    <Name>
                                       <StringValue>i2</StringValue>
                                    </Name>
                                 </LookupNow>
                              </NumericRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" LineNo="29" ColNo="4">
                        <NodeId>ASSIGNMENT__4</NodeId>
                        <NodeBody>
                           <Assignment>
                              <StringVariable>s1</StringVariable>
                              <StringRHS>
                                 <LookupNow>
                                    <Name>
                                       <StringValue>s1</StringValue>
                                    </Name>
                                 </LookupNow>
                              </StringRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" LineNo="31" ColNo="4">
                        <NodeId>ASSIGNMENT__5</NodeId>
                        <NodeBody>
                           <Assignment>
                              <StringVariable>s2</StringVariable>
                              <StringRHS>
                                 <LookupNow>
                                    <Name>
                                       <StringValue>s2</StringValue>
                                    </Name>
                                 </LookupNow>
                              </StringRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" LineNo="33" ColNo="4">
                        <NodeId>ASSIGNMENT__6</NodeId>
                        <NodeBody>
                           <Assignment>
                              <BooleanVariable>b1</BooleanVariable>
                              <BooleanRHS>
                                 <LookupNow>
                                    <Name>
                                       <StringValue>b1</StringValue>
                                    </Name>
                                 </LookupNow>
                              </BooleanRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" LineNo="35" ColNo="4">
                        <NodeId>ASSIGNMENT__7</NodeId>
                        <NodeBody>
                           <Assignment>
                              <BooleanVariable>b2</BooleanVariable>
                              <BooleanRHS>
                                 <LookupNow>
                                    <Name>
                                       <StringValue>b2</StringValue>
                                    </Name>
                                 </LookupNow>
                              </BooleanRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                  </NodeList>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="40" ColNo="8">
               <NodeId>TestString1</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="sibling">TestVars1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <PostCondition>
                  <AND>
                     <IsKnown>
                        <StringVariable>s</StringVariable>
                     </IsKnown>
                     <EQString>
                        <StringVariable>s</StringVariable>
                        <StringValue>yes</StringValue>
                     </EQString>
                  </AND>
               </PostCondition>
               <NodeBody>
                  <Command>
                     <StringVariable>s</StringVariable>
                     <Name>
                        <StringValue>get_string</StringValue>
                     </Name>
                  </Command>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="45" ColNo="8">
               <NodeId>TestString2</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="sibling">TestString1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <PostCondition>
                  <NOT>
                     <IsKnown>
                        <StringVariable>s</StringVariable>
                     </IsKnown>
                  </NOT>
               </PostCondition>
               <NodeBody>
                  <Command>
                     <StringVariable>s</StringVariable>
                     <Name>
                        <StringValue>get_string</StringValue>
                     </Name>
                  </Command>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="50" ColNo="8">
               <NodeId>TestInt1</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="sibling">TestString2</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <PostCondition>
                  <AND>
                     <IsKnown>
                        <IntegerVariable>i</IntegerVariable>
                     </IsKnown>
                     <EQNumeric>
                        <IntegerVariable>i</IntegerVariable>
                        <IntegerValue>1</IntegerValue>
                     </EQNumeric>
                  </AND>
               </PostCondition>
               <NodeBody>
                  <Command>
                     <IntegerVariable>i</IntegerVariable>
                     <Name>
                        <StringValue>get_int</StringValue>
                     </Name>
                  </Command>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="56" ColNo="8">
               <NodeId>TestInt2</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="sibling">TestInt1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <PostCondition>
                  <NOT>
                     <IsKnown>
                        <IntegerVariable>i</IntegerVariable>
                     </IsKnown>
                  </NOT>
               </PostCondition>
               <NodeBody>
                  <Command>
                     <IntegerVariable>i</IntegerVariable>
                     <Name>
                        <StringValue>get_int</StringValue>
                     </Name>
                  </Command>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="61" ColNo="8">
               <NodeId>TestReal1</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="sibling">TestInt2</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <PostCondition>
                  <AND>
                     <IsKnown>
                        <RealVariable>r</RealVariable>
                     </IsKnown>
                     <EQNumeric>
                        <RealVariable>r</RealVariable>
                        <RealValue>1.0</RealValue>
                     </EQNumeric>
                  </AND>
               </PostCondition>
               <NodeBody>
                  <Command>
                     <RealVariable>r</RealVariable>
                     <Name>
                        <StringValue>get_real</StringValue>
                     </Name>
                  </Command>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="67" ColNo="8">
               <NodeId>TestReal2</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="sibling">TestReal1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <PostCondition>
                  <NOT>
                     <IsKnown>
                        <RealVariable>r</RealVariable>
                     </IsKnown>
                  </NOT>
               </PostCondition>
               <NodeBody>
                  <Command>
                     <RealVariable>r</RealVariable>
                     <Name>
                        <StringValue>get_real</StringValue>
                     </Name>
                  </Command>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="72" ColNo="8">
               <NodeId>TestBool1</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="sibling">TestReal2</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <PostCondition>
                  <AND>
                     <IsKnown>
                        <BooleanVariable>b</BooleanVariable>
                     </IsKnown>
                     <NOT>
                        <BooleanVariable>b</BooleanVariable>
                     </NOT>
                  </AND>
               </PostCondition>
               <NodeBody>
                  <Command>
                     <BooleanVariable>b</BooleanVariable>
                     <Name>
                        <StringValue>get_bool</StringValue>
                     </Name>
                  </Command>
               </NodeBody>
            </Node>
            <Node NodeType="Command" LineNo="78" ColNo="8">
               <NodeId>TestBool2</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeStateVariable>
                        <NodeRef dir="sibling">TestBool1</NodeRef>
                     </NodeStateVariable>
                     <NodeStateValue>FINISHED</NodeStateValue>
                  </EQInternal>
               </StartCondition>
               <PostCondition>
                  <NOT>
                     <IsKnown>
                        <BooleanVariable>b</BooleanVariable>
                     </IsKnown>
                  </NOT>
               </PostCondition>
               <NodeBody>
                  <Command>
                     <BooleanVariable>b</BooleanVariable>
                     <Name>
                        <StringValue>get_bool</StringValue>
                     </Name>
                  </Command>
               </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>