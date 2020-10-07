# Logbog




### Logbog 18/9:
Vi er startet ud med et miniprojekt hvor vi skal løse programmeringspuslespil i toms dataløg. 

En af de første dele jeg gik igang med var at kunne indlæse filen og isolere *payloaded* dvs. en ascii85 kodet datastreng. Dette blev gjort igennem brug at STL bibliotekets `ifstream`. Derefter blev der lavet en funktion, som kunne fjerne beskeden fra laget. Denne funktion blev senere udvidet til at at kunne fjerne newlines, da disse var tilføjet inde i den kodet streng pga. visuelle hensyn, hvilket decoderen ikke var forberedt på.

Ascii85 decoderen er baseret på standarden angivet i wikipedia artiklen, og bruger bla. bitshifting til at udtrække bogstaverne fra den samlede int.

### Logbog 28/9:
I dag startede jeg på lag 1 i toms dataløg. Dette lag handlede om at benytte de bitwise operationer som er i c++. Man skulle for hvert byte, flippe hvert andet bit, og rotere bitsne en til højre, hvor den bit længst til højre blev sat ud foran. For at flippe hvert andet bit, blev der brugt en XOR, hvor hvert bit blev XOR'et med `0b01010101`. XOR returnere sandt hvis en af bitsne er 1 og falsk hvis begge bits er 0 eller 1. Deraf må det gælde at bitsne bliver vendt da hvis bitsne i data bittet er 1 bliver det til 0 og hvis det er 0 bliver det til 1. 

For at rotere blev der brugt en AND maske til at hive det relevante bit ud, hvorefter det blev shiftet til fronten i sit eget byte. Resten af bitsne blev shiftet en til højre og det resterende bit blev AND'et på igen.

Der blev også startet på lag 2 her, men det bliver fulgt op på i næste logbogsafsnit.

### Logbog 02/10:
I dag* startede vi på lag 2 i toms dataløg. Her handlede det om at tjekke parity bits og finde korrupte bytes, som skulle filtreres fra inden den færdige besked kunne findes.
Algoritmen til at afkode beskeden havde følgende trin:
- For hvert byte, tjek om det beregnede og den angivne parity bit matcher. Hvis den ikke gør dette så fjern bytet fra strengen.
- Med den nye streng kan man hive en chunk af 8 bytes ud, fjerne parity bitsne og sætte den sammen til 7 bytes, som er bogstaverne i ens besked.



For at opnå dette formål blev der brugt bitsets, bitshifting og en OR operation til at sætte de 8 bytes sammen til 7.
