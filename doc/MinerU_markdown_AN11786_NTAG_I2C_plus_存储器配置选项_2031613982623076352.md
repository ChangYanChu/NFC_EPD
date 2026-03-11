# AN11786

NTAG I²C plus memory configuration options 

Rev. 1.1 — 2 June 2018 358711 

Application note COMPANY PUBLIC 


Document information


<table><tr><td>Info</td><td>Content</td></tr><tr><td>Keywords</td><td>NTAG I2C plus, memory configuration, NDEF, read-only, EEPROM</td></tr><tr><td>Abstract</td><td>Describe how to configure the NTAG I2C plus in the different use case for read/write, read-only and protected access from RF and I2C side</td></tr></table>


Revision history


<table><tr><td>Rev</td><td>Date</td><td>Description</td></tr><tr><td>1.1</td><td>20180502</td><td>Editorial update and clarification on I²C address added</td></tr><tr><td>1.0</td><td>20160201</td><td>Initial revision</td></tr></table>

# Contact information

For more information, please visit: http://www.nxp.com 

# 1. Introduction

The NTAG I²C plus ([2]) extends the NTAG I²C with several options to protect the access to the memory. The memory by default is read/write accessible and can be flexible protected to read-only, password protected write and password protected read/write. 

The following use-cases should be described: 

• Read only protection 

• Partially password protect the memory: 

o split first part in unprotected/protected (keep in mind lock bit granularity) 

o use Sector 1 as password protected Memory (protect the write or read/write with a 32-bit password) 

• Use a part of the memory as configuration EEPROM (only accessible from I²C) for e.g. MCUs 

• Partial RF only access to Memory using the I²C lock bits 

Additionally, it is important to know how to calculate the available NDEF size and put this information in the Capability Container, this is described first and then the different use cases are described. 

# 2. Memory Layout of the NTAG I²C plus

If NDEF should be used on the tag, then the NDEF message always has to start at begin of the memory in user memory page 04 unless there are additional memory or lock control TLVs in front of the NFC message. Page 03 contains the NFC Forum capability container and contains the maximum available user memory size for storing a NDEF message. 

How to define the NDEF message area in a NFC Forum Type 2 Tag is specified in [1]. Here a short practical introduction in calculating the capability container content should be given. 

The Capability container in page 03 consists of four bytes. An example is given in the table below. 


Table 1. Capability Container of a NFC Forum Type 2 Tag


<table><tr><td>Byte</td><td>Byte 0</td><td>Byte 1</td><td>Byte 2</td><td>Byte 3</td></tr><tr><td>Data/Content</td><td>0xE1</td><td>0x10</td><td>0x6D</td><td>0x00</td></tr><tr><td>Meaning</td><td>Magic Number for NDEF</td><td>NDEF mapping version 1.0</td><td>NDEF memory size in units of 8 bytes</td><td>NDEF access indicator, 0 means free access</td></tr></table>

Byte 2 contains the size of the memory map which can be used for NDEF storage. As long as there are no holes in this area (needed for the NTAG I²C plus 2k) the maximum size available for storing NDEF TLVs (T2T_Area) is given by: 

$$
\mathrm {T 2 T} \_ \text {A r e a} = (\text {B y t e} 2 \text {o f C C}) ^ {*} 8
$$

The T2T_Area starts at page 04 and extends as many bytes as given by the NDEF memory size byte in the capability container. 

To know the actual space for storing NDEF TLVs possible holes defined in Memory Control TLV and Lock Control TLVs have to be deducted from that space. 

A memory initialization of a NTAG I²C plus with NDEF for a user memory size like the NTAG 216(F) is given in the below table. This initialization is recommended to be used on NTAG I²C plus 1k/2k unless there are special needs. When NDEF messages need more space, following changes need to be applied according to NFC Forum Type 2 Tag specification: 

• CC, Byte2 (size of T2T_Area) 

• Lock Control TLV needs to be added to address location of Dynamic Lock bits and their granularity 

• Memory Control TLV needs to be added to exclude Configuration memory area and SRAM area at the end of Sector 0 


Table 2. Memory initialization example for NTAG I²C plus for NDEF message size like on NTAG216


<table><tr><td>Page Address</td><td colspan="4">Byte number within page</td></tr><tr><td></td><td>0</td><td>1</td><td>2</td><td>3</td></tr><tr><td>03h</td><td>E1h</td><td>10h</td><td>6Dh</td><td>00h</td></tr><tr><td>04h</td><td>03h</td><td>00h</td><td>FEh</td><td>00h</td></tr></table>

More details about how the memory can be configured to maximize the compatibility and locked to read only in a compatibility optimized way is described in [3]. 

NOTE: When configuring Block 0 of NTAG I²C plus from I²C perspective, also I²C write address gets updated, because I²C write address is stored in the first byte of user memory. Reading out this first byte of NTAG I²C plus, it always returns 04h (UID0). Therefore, for convenience reasons, it is recommended to change default I²C device address 55h to 02h. As this first byte codes I²C write address, 04h (02h shifted left by one bit) value needs to be written to Byte 0. 

# 3. Use Cases.

# 3.1 Read Only Protection

The memory can be configured to be read only from the NFC side using the static and dynamic lock bits. The static lock bits can set the pages from 03h to 0Fh with a one-page granularity to read only. The dynamic lock bits lock the rest of the user memory with a granularity depending on the NTAG I²C plus memory size (1k or 2k). 

If the lock bits are set, then RF cannot write to the corresponding pages anymore. The NFC side cannot unset the lock bits but the I²C side still can write to the memory even if the lock bits are set. I²C also can also reset the lock bits if write access from the NFC side to the corresponding pages is needed again. 

So, if the content of read-only pages should be updated, either I²C needs to unset the lock bits and the NFC side can write again into the memory, or the data to be written is transferred using e.g. the pass-through mode to the MCU and the MCU writes the data into the memory over I²C. 

The read only protection is independent from the other use cases shown below and can be freely combined with them. Only the recommendation for setting the size of the protected area below should be considered. 

When protecting a tag to read only also the configuration lock bits (REG_LOCK_I2C and REG_LOCK_RF) should be considered and set if the configuration should be frozen. This two configuration lock bits once set remain set forever, they cannot be cleared. 

# 3.2 Password Protection

The NTAG I²C plus features a password protection like the NTAG 21x(F). In the Sector 0 the password protection of the NTAG I²C plus works exactly like known from the NTAG 21x(F) where the relevant configuration parameters are: 

• Protection pointer (AUTH0), which defines where the protected memory starts. If AUTH0 is set greater than EBh then password protection is disabled 

• Protection mode (bit “NFC_PROT” in the configuration area), which defines if the protected area needs the password only for writing, or also for reading the memory 

• Bit SRAM_PROT which enables/disables the password protection of read and write accesses to SRAM that includes pass-through mode 

• Bit 2K_PROT which enables or disables password protection for the whole Sector 1 of the NTAG I²C plus 2K 

For selecting the size of the protected area, these points should be considered as well: 

• If either the protected area should be at the same time locked to read-only or a read-only area is directly before the protected area, it is necessary to align the protected area with the given granularity of the dynamic lock bits which is: 

o 16 pages per lock bit for the NTAG I²C plus 1k 

o 32 pages per lock bit for the NTAG I²C plus 2k 

• If the T2T_Area (defined by the capability container) overlaps with the protected area, the start of the protected area should be aligned with lock bit granularity. In addition, a Memory Control TLV is needed to exclude the protected area from the T2T_Area. 

• Dynamic Lock Bits are always on the same memory location. To be compliant to NFC Forum Type 2 Tag, in addition a Lock Control TLV is needed. 

# 3.3 I²C only access

On the NTAG I²C plus 2k the Sector 1 can be switched unavailable for NFC access using the bit NFC_DIS_SEC1. When this bit is set to 1b, then NFC has no access to the whole Sector 1 anymore and this memory can then be used as I²C only accessible EEPROM starting from Block 64/40h to Block 127/7Fh. 

This EEPROM area can be accessed with the normal NTAG I²C plus I²C read and write commands which read/write data in units of 16 bytes. 

# 3.4 RF only access / I²C access protection

With the setting I2C_PROT the protected area defined by: 

• All Memory above AUTH0 (page address in Sector 0) up to Block 58/3Ah (dynamic lock bits are excluded) 

• If 2K_PROT is set the corresponding I²C blocks from NFC Sector 1 (Block 64/40h up to Block 127/7Fh) 

Can be set to these modes: 

• 00b: I²C has full read/write access to this area 

• 01b: I²C can only read the protected area and read/write the remaining memory 

• 1xb: I²C cannot access the protected area at all (only NFC can access the protected area) 

With this mechanism a part of the memory can be specified which is only accessible via NFC. 

# 4. References

[1] Type 2 Tag Operation, Technical Specification, Version 1.2, 2014-03-27, [T2TOP] 

[2] NT3H2111/NT3H2211, NTAG I²C plus, NFC Forum Type 2 Tag compliant IC with I²C interface http://www.nxp.com/documents/data_sheet/NT3H2111_2211.pdf 

[3] AN11456, NTAG Using the dynamic lock bits to lock the tag 

# 5. Legal information

# 5.1 Definitions

Draft — The document is a draft version only. The content is still under internal review and subject to formal approval, which may result in modifications or additions. NXP Semiconductors does not give any representations or warranties as to the accuracy or completeness of information included herein and shall have no liability for the consequences of use of such information. 

# 5.2 Disclaimers

Limited warranty and liability — Information in this document is believed to be accurate and reliable. However, NXP Semiconductors does not give any representations or warranties, expressed or implied, as to the accuracy or completeness of such information and shall have no liability for the consequences of use of such information. NXP Semiconductors takes no responsibility for the content in this document if provided by an information source outside of NXP Semiconductors. 

In no event shall NXP Semiconductors be liable for any indirect, incidental, punitive, special or consequential damages (including - without limitation - lost profits, lost savings, business interruption, costs related to the removal or replacement of any products or rework charges) whether or not such damages are based on tort (including negligence), warranty, breach of contract or any other legal theory. 

Notwithstanding any damages that customer might incur for any reason whatsoever, NXP Semiconductors’ aggregate and cumulative liability towards customer for the products described herein shall be limited in accordance with the Terms and conditions of commercial sale of NXP Semiconductors. 

Right to make changes — NXP Semiconductors reserves the right to make changes to information published in this document, including without limitation specifications and product descriptions, at any time and without notice. This document supersedes and replaces all information supplied prior to the publication hereof. 

Suitability for use — NXP Semiconductors products are not designed, authorized or warranted to be suitable for use in life support, life-critical or safety-critical systems or equipment, nor in applications where failure or malfunction of an NXP Semiconductors product can reasonably be expected to result in personal injury, death or severe property or environmental damage. NXP Semiconductors and its suppliers accept no liability for inclusion and/or use of NXP Semiconductors products in such equipment or applications and therefore such inclusion and/or use is at the customer’s own risk. 

Applications — Applications that are described herein for any of these products are for illustrative purposes only. NXP Semiconductors makes no representation or warranty that such applications will be suitable for the specified use without further testing or modification. 

Customers are responsible for the design and operation of their applications and products using NXP Semiconductors products, and NXP Semiconductors accepts no liability for any assistance with applications or customer product design. It is customer’s sole responsibility to determine whether the NXP Semiconductors product is suitable and fit for the customer’s applications and products planned, as well as for the planned application and use of customer’s third party customer(s). Customers should provide appropriate design and operating safeguards to minimize the risks associated with their applications and products. 

NXP Semiconductors does not accept any liability related to any default, damage, costs or problem which is based on any weakness or default in the customer’s applications or products, or the application or use by customer’s third party customer(s). Customer is responsible for doing all necessary testing for the customer’s applications and products using NXP Semiconductors products in order to avoid a default of the applications and the products or of the application or use by customer’s third party customer(s). NXP does not accept any liability in this respect. 

Export control — This document as well as the item(s) described herein may be subject to export control regulations. Export might require a prior authorization from competent authorities. 

Translations — A non-English (translated) version of a document is for reference only. The English version shall prevail in case of any discrepancy between the translated and English versions. 

Evaluation products — This product is provided on an “as is” and “with all faults” basis for evaluation purposes only. NXP Semiconductors, its affiliates and their suppliers expressly disclaim all warranties, whether express, implied or statutory, including but not limited to the implied warranties of noninfringement, merchantability and fitness for a particular purpose. The entire risk as to the quality, or arising out of the use or performance, of this product remains with customer. 

In no event shall NXP Semiconductors, its affiliates or their suppliers be liable to customer for any special, indirect, consequential, punitive or incidental damages (including without limitation damages for loss of business, business interruption, loss of use, loss of data or information, and the like) arising out the use of or inability to use the product, whether or not based on tort (including negligence), strict liability, breach of contract, breach of warranty or any other theory, even if advised of the possibility of such damages. 

Notwithstanding any damages that customer might incur for any reason whatsoever (including without limitation, all damages referenced above and all direct or general damages), the entire liability of NXP Semiconductors, its affiliates and their suppliers and customer’s exclusive remedy for all of the foregoing shall be limited to actual damages incurred by customer based on reasonable reliance up to the greater of the amount actually paid by customer for the product or five dollars (US$5.00). The foregoing limitations, exclusions and disclaimers shall apply to the maximum extent permitted by applicable law, even if any remedy fails of its essential purpose. 

# 5.3 Licenses

# Purchase of NXP ICs with NFC technology

Purchase of an NXP Semiconductors IC that complies with one of the Near Field Communication (NFC) standards ISO/IEC 18092 and ISO/IEC 21481 does not convey an implied license under any patent right infringed by implementation of any of those standards. Purchase of NXP Semiconductors IC does not include a license to any NXP patent (or other IP right) covering combinations of those products with other products, whether hardware or software. 

# 5.4 Trademarks

Notice: All referenced brands, product names, service names and trademarks are property of their respective owners. 

NTAG — is a trademark of NXP B.V. 

I²C-bus — logo is a trademark of NXP B.V 

# 6. Contents

1. Introduction ... 

2. Memory Layout of the NTAG I²C plus ................3 

3. Use Cases. .... 

3.1 Read Only Protection .. 4 

3.2 Password Protection .. .5 

3.3 I²C only access.... .5 

3.4 RF only access / I²C access protection .............. 5 

4. References... 

5. Legal information .. 8 

5.1 Definitions . .8 

5.2 Disclaimers... .8 

5.3 Licenses. .. 8 

5.4 Trademarks.. .8 

6. Contents... 

Please be aware that important notices concerning this document and the product(s) described herein, have been included in the section 'Legal information'. 

© NXP B.V. 2018. 

For more information, visit: http://www.nxp.com 

All rights reserved. 

Date of release:2 June 2018 

358711 

Document identifier: AN11786 