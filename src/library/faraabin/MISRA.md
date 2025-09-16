# MISRA Compliance

The faraabin library files conform to the [MISRA C:2012](https://www.misra.org.uk/misra-c)
guidelines, with the deviations listed below.

#### Rule 3.1

- This rule states that the character sequences '/*' and '//' must not be used within comments. 
- In the comment sections of the Faraabin files, references to the GitHub and website of Faraabin are included,
  which do not comply with this rule.

#### Rule 7.4
- This rule states that a string literal shall not be assigned to an object unless the object’s type is pointer to const-qualified char.
- 

### Rule 10.3

- This rule states that the value of an expression must not be assigned to an object with a narrower *essential type* or a different *essential type category*.
- In various parts of the code, constant numbers are assigned to variables. Since it is not possible to explicitly enforce a number to be of types such as 'uint8_t', static analyzers flag this as a violation when these numbers are assigned to the corresponding variables.

### Rule 10.4

- This rule states that both operands of an operator must have the same *essential type category*.
- The reason for the violation is the same as in Rule 10.3.

#### Rule 10.8

- This rule states that the value of an expression shall not be assigned to an object with a narrower essential type.
- The CPU Profiler add-on in Faraabin calculates CPU usage as a floating-point value. However, this value is sent to Faraabin
  as a 'uint16_t' or 'uint32_t'. Since CPU usage cannot be negative, this assignment does not pose a problem.

#### Rule 11.1

- This rule states that conversions shall not be performed between a pointer to a function and any other type.
- In Faraabin, pointers to all necessary functions are recorded in the Faraabin database for internal use. Each of these functions is fully recognized by the library, and whenever the application intends to call them, their existence is already established. Therefore, casting these pointers will not lead to undefined behavior in the code.

#### Rule 11.4

- This rule states that a pointer should not be converted to an integer, and an integer should not be converted to a pointer.
  Such conversions may result in undefined behavior.
- As explained in the rationale section of the MISRA guidelines, while a cast may be necessary in some cases,
  care should be taken to ensure that pointers do not lead to undefined behavior.
- In Faraabin, all fobjects are identified by their addresses in the Faraabin database. Each of these is completely known to the library,
  and whenever the application wants to send a command to a fobject, its type is already known. Therefore, casting these addresses
  will not result in undefined behavior in the code.

#### Rule 11.5

- This rule states that conversion from a pointer to void into a pointer to an object should not be performed.
- In the rationale section, MISRA explains that if this conversion is used, care should be taken to ensure it does not result in undefined behavior.
- In the Faraabin library, functions that generate corresponding frames for each fobject capture the payload using a pointer to void.
  This approach enhances code readability, and each function casts the pointer to the correct object, thereby preventing undefined behavior.

#### Rule 11.6

- This rule states that a cast shall not be performed between a pointer to 'void' and an arithmetic type, as it may result in undefined behavior.
- Pointers to states and transitions in a state machine fobject are transmitted to Faraabin as a 'uint32_t' type. This is a well-defined subject in the Faraabin library, ensuring that no undefined behavior will occur.

#### Rule 15.4

- This rule states that a loop should have no more than one *break* statement.
- In this library loop are permitted to break by satisfying different conditions.
  This deviation from the standard rule is sanctioned in our coding style to enhance readability.

#### Rule 15.5

- This rule states that a function should have no more than one *return* statement.
- In this library functions are permitted to return prematurely if the conditions necessary for code execution
  are not met. This deviation from the standard rule is sanctioned in our coding style to enhance readability.

#### Rule 17.1

- This rule states that features of *<stdarg.h>* shall not be used.
- For printing user events in a string format, similar to the 'printf' function, Faraabin utilizes these features.

#### Rule 19.2

- This rule states that the *union* keyword should not be used.
- The types 'uByte2', 'uByte4', and 'uByte8', which are defined in the 'faraabin_type.h' header, are unions with members of the same size.
In this case, no padding is inserted at the end of the union, and the bit order, endianness, and alignment of the members are the same.


#### Rule 21.3

- This rule states that the 'malloc' function shall not be used.
- In the Faraabin library, users can allocate memory for the capture and stream buffers of a databus or Faraabin link in two different ways:
  1. **Static Allocation**: The user must create these buffers and define them to Faraabin via their pointers.
  2. **Dynamic Allocation**: The user only specifies the buffers' size. Faraabin allocates the buffer in heap using 'malloc'.
  
If the user utilizes the static allocation method, this rule will not be violated.

#### Rule 21.6

- This rule states that the 'vsnprintf' function shall not be used.
- For printing user events in a string format, similar to the 'printf' function, Faraabin utilizes this function.

