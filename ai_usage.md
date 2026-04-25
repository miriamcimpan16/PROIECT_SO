AI Usage Documentation-Phase 1

Tool-uri folosite:

Claude - pentru generarea inițială a funcțiilor de bază (parse_condition și match_condition).

Gemini - pentru asistență în înțelegerea și depanarea problemei de memorie statică a funcției strtok.

Prompt-urile folosite:
Prompt 1:

"Hi! I am a Computer Science student in 2nd year, and I ask you to generate a function with this signature: int parse_condition(const char *input, char *field, char *op, char *value);, that splits the input in three strings. For example: "severity:>:2" in field = "severity", op = ">=" and value = "2". After that explain to me how you do it, step by step. Use strtok, because I'm most familiar with this one. And make it as simple as you can."

Prompt 2:

"I have the following C structure for a report stored in a binary file: [structura report]. I need a C function: int match_condition(report *r, const char *field, const char *op, const char *value);. This function returns 1 if the report matches the condition and 0 otherwise. Conditions are given like this field:operator:value, already split into field, op, value. Convert value to the correct type, use strcmp for string comparison. For string fields, support only == and !=. Supported fields: severity(int), category(string, stored in r->issue), inspector(string, stored in r->nume), timestamp(time_t). Write simple code, and explain it to me step by step."

Analiza codului generat:
Pentru prima funcție (parse_condition): Claude a generat o funcție bazată pe strtok așa cum am cerut. Aceasta despărțea textul la caracterul : și "așeza" bucățile rezultate în câmpurile destinație (field, op, value) prin calcularea pozițiilor în string.

Pentru a doua funcție (match_condition): A generat corect un lanț de instrucțiuni if-else care folosea atoi() și conversii (time_t)atol() pentru numere și strcmp() pentru șiruri de caractere. Totuși, a omis consistența operatorilor de egalitate (folosind uneori = în loc de ==).

Probleme întâmpinate și soluții:
Deși codul inițial părea funcțional în izolare, am întâmpinat probleme critice la integrarea lui în programul principal:

Corectarea operatorului de egalitate (= în ==): Am adăugat manual o condiție care face automat conversia: if(strcmp(op,"=") == 0) strcpy(op,"==");. Această modificare a fost necesară pentru a asigura compatibilitatea între formatul extras de prima funcție și verificările din a doua funcție.

Bug-ul de memorie statică la strtok: Când am implementat comanda filter pentru a suporta mai multe condiții, am folosit strtok(input, " ") într-o buclă pentru a le separa. În interiorul acestei bucle, apelam parse_condition, care la rândul ei folosea strtok.

Problema: Bucla principală se oprea mereu după prima condiție, a doua fiind complet ignorată.

Explicația tehnică: Funcția strtok folosește un pointer static intern (un "semn de carte") pentru a ține minte unde a rămas. Apelul imbricat din parse_condition suprascria acest pointer. La revenirea în bucla principală, strtok primea NULL prematur.

Soluția cu strchr: Pentru a repara bug-ul, am rescris funcția parse_condition folosind strchr. Spre deosebire de strtok, strchr este stateless (nu are memorie ascunsă), căutând doar poziția unui caracter. Acest lucru a permis iterarea corectă prin toate condițiile de filtrare.

Ce am învățat:
Înțelegerea programului este importanta: Generarea unei funcții corecte în izolare nu garantează funcționarea ei într-un program complex. 

Funcții Stateless vs. Stateful: Am înțeles de ce strtok nu este "thread-safe" sau "reentrant" și cum apelurile sale imbricate pot distruge fluxul unui program.

Rolul AI-ului: Am realizat că AI-ul este un asistent util, dar responsabilitatea arhitecturii și a depanării rămâne la programator. Trebuie să fim capabili să analizăm critic orice cod generat.