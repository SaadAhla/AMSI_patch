##### Inside AmsiOpenSession, there is a `TEST` instruction that sets the zero flag (`ZF`) , when the result of the AND operation is zero, and if the zero flag is 1 it will take the error branch because of the `JZ` instruction that will jump if `ZF` is 1 , but if everything is ok the error branching will never took , so what about forcing it by patching `JZ` to `JNZ`.
##### N.B : `JZ` is similar to `JE` and `JNZ` is similar to `JNE` :

![image](https://user-images.githubusercontent.com/123980007/216672566-fa50d6e5-0c80-4b33-83c3-02bdee2fe8b6.png)

##### You can see after patching `JE` to `JNE` using windbg , the Error branching is forced and AMSI is patched :

![image](https://user-images.githubusercontent.com/123980007/216671477-b7a4eaba-b62d-4196-97f9-9fcc6daf0450.png)


![image](https://user-images.githubusercontent.com/123980007/216676689-14f1187a-2709-4ada-9b09-f7a76bc9fd06.png)
