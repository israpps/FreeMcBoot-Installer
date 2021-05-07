PlayStation 2 Identification tool (PS2Ident) v0.835	- 2018/12/08
====================================================================

PS2Ident is an identification tool that allows dumps of a PlayStation 2 console's ROM chips and MECHACON NVRAM to be made.
It will also gather data from the console, for research purposes.

It has the following features:

*Dumps the boot ROM chip.
*Dumps the whole DVD ROM (rom1:, rom2: and erom: in one file) chip.
*Displays the actual addresses for DEV1 (rom1, rom2 and erom) that are set by the ROM filesystem drivers.
*Coloured user interface that is easy to use.
*Supports dumping to memory cards and USB mass storage devices.
*Supports multi-languages, which include the 8 supported languages by the PS2
*Gathers data of all known parts of the PS2.
*Attempts to automatically match the chip/part name with the version number of the part.
*Supports all PlayStation 2 consoles, including the SCPH-10000, SCPH-15000 and SCPH-50009, and the PSX (DVR unit).

How to use this software:
---------------------------
Extract the whole archive (do not delete any files or folders) onto a location on a supported device.
The only supported devices are the Memory Card and USB mass storage device.

When the console's mainboard cannot be identified, the mainboard model will be "unknown".
A database record file (*_database.bin) will be generated when a dump is made. No personal information is recorded.
When making a submission (only when your mainboard is not recognized),
please provide the database record file and your console's mainboard model.
If the mainboard model is not provided, your contribution will not be accepted.

If possible, please help to complete the chip ID list as well,
for any part that is not identified ("unknown") or is incorrectly identified.
When doing so, please provide the full model of the part, especially for the MECHACON chip (e.g. CXP103049-401GG).

As for the chassis and EMCS ID (For Dragon-series units):
	At the lower right-hand corner of the product information sticker that is either on the bottom
	or back of the unit, there will be something like "H FOXC".
	In this example, the letter 'H' is the chassis model,
	while "FOXC" stands for FOXConn (the EMCS that made the console).

Notes:
	1. Chip and mainboard identification is currently very incomplete and inaccurate, due to a lack of data.
		Sometimes, Sony makes hardware revisions without changing the chip implementation numbers as well,
		hence why chip identification may be inaccurate.
		The chip and version IDs are, however, accurate since they are taken directly from the hardware.
	2. Slimline consoles that had the SSBUS I/F Controllers integrated into the IOP will report them as version 0x31.
	3. Due to some SSBUS I/F controllers reporting the same version number (0x31), those will be all generalized
		and reported as "CXD9611". All of them are supposed to be fully compatible anyway.

Mainboards are uniquely identified by:
	ROMVER string
	Model name
	EE revision
	FPU revision
	IOP revision
	GS revision
	MECHACON revision and region
	SPU2 revision
	SSBUS I/F Controller revision
	Model ID
	Console model ID
	EMCS ID
	M Renewal Date
	ADD0x010
	BOOT + DVD ROM Checksum

Identified data:
	Emotion Engine (EE)		- "main" processor of the PlayStation 2.
	I/O Processor (IOP)		- "sub" processor of the PlayStation 2. Most peripherals are connected to this.
	Sound Processor Unit 2 (SPU2)	- Sound processor.
	Graphics Synthesizer (GS)	- Graphics processor.
	MECHAnics CONtroller (MECHACON)	- Drive mechanics controller.
	SS-BUS InterFace Controller	- InterFace (I/F) controller for the DEV9 expansion device.
	SPEED chip			- The controller chip on an expansion device (e.g. Network Adaptor).
					  It's not known what "SPEED" stands for.
	Ethernet PHY			- The Ethernet PHYsical transceiver on a network adaptor.
	i.Link controller		- The IEEE1394 controller that is integrated into the IOP chip.
	USB controller			- The OHCI USB controller that is integrated into the IOP chip.
	Boot ROM			- Read-Only Memory (ROM) chip containing the kernels and basic IOP modules.
	DVD ROM				- Read-Only Memory (ROM) chip containing the DVD player.
	NVM/EEPROM			- The Non-Volatile Memory storage that the MECHACON has,
					  for storing settings (system, RTC and CD/DVD drive).
	ROMVER				- The ROM VERsion string that identifies the version and build date of the
					   boot ROM.
	ROMGEN				- The ROM GENeration number that identifies the date that the boot ROM
					   was generated.
	AIF				- On a DTL-T10000(H), the DEV9 interface is connected through the AIF.
					  The AIF also provides a RTC and IDE controller.	
	Machine Type			- Software value that determines the operating mode of the EE kernel,
					  hence machine dependent.
	Model name			- The name given by SCE for the console. The SCPH-10000,
					  SCPH-15000 and DTL-H10000 will always show up as "SCPH-10000".
	Chassis				- The internal build/model/type of the console.
	MPU board			- See MRP.
	MRP (BoardInf)			- The MRP is the interface between the PS2 and PC sides of a DTL-T10000(H).
					  All DTL-T10000(H) units have a MPU 4.0 board as the MRP.
	i.Link ID			- Contains the part of the IEEE1394 EUI-64 address that uniquely identifies
					  the console, the model ID and EMCS ID.
	EMCS ID				- Uniquely identifies the Engineering, Manufacturing and Customer Services (EMCS)
					  system that made the console.
	Model ID			- Uniquely identifies the console's model. Even more accurately than the
					  model name.
	Console ID			- Contains the EMCS ID, (another version of) model ID and serial number.
	M Renewal Date			- What this represents is unclear. It's something related to the
						MECHACON (date of firmware build or EEPROM settings update?)
	ADD0x010			- An ID that is used by the SONY service tools to identify major revisions.
						Originally at word 0x010 of the EEPROM, it was moved to word 0x01 for the Dragon models.
						It does not always correspond with the chassis models (i.e. C and D-chassis have the same ID).
						It can also identify the loadout of the console,
						like its CEX/DEX status and the OP block installed (i.e. SANYO or SONY).
	PS1DRV Version			- The version number of the PlayStation driver. The earliest ROMs do not have PS1VER in ROM.
					  For Japan, only the SCPH-10000 and SCPH-15000 lack this file. "1.01" is hardcoded.
					  For other territories (should be only the earliest SCPH-30001), "1.10" is hardcoded.
	DVD Player Version		- The version number of the built-in DVD player, if applicable.

Known chassis versions (this list is not complete):
	A-chassis	GH-001	SCPH-10000
	A-chassis	GH-003	SCPH-15000
	A-chassis+	GH-003	SCPH-18000
	AB-chassis	GH-008	SCPH-18000
	B-chassis	GH-004	SCPH-30001
	B'-chassis	GH-005	SCPH-30001
	C-chassis	GH-006	SCPH-30001-4
	C'-chassis	GH-007	SCPH-30002-4
	D-chassis	GH-010	SCPH-30001D-SCPH-30002D
	D'-chassis	GH-012	SCPH-30003D'-SCPH-30004D'
	D-chassis	GH-014	SCPH-30000-30004, SCPH-35000-35004, SCPH-30001R - SCPH-30004R
	D-chassis	GH-016	SCPH-30000-30004, SCPH-35000-35004, SCPH-30001R - SCPH-30004R
	F-chassis	GH-015	SCPH-30000, SCPH-30001R - SCPH-30007R. New CXP103049 MECHACON
	G-chassis	GH-017	SCPH-39000-4,SCPH-37000L,SCPH-37000B
	G-chassis	GH-019	SCPH-39000-4,SCPH-37000L,SCPH-37000B
	G-chassis	GH-022	SCPH-39000-4,SCPH-37000L,SCPH-37000B
	H-chassis	GH-023	SCPH-50000-11
	I-chassis	GH-026	SCPH-50000-11
	K-chassis	GH-032	SCPH-70000-12
	K-chassis	GH-035	SCPH-70000-12
	L-chassis	GH-037	SCPH-75000-12
	L-chassis	GH-040	SCPH-75000-12
	L-chassis	GH-041	SCPH-75000-12
	M-chassis	GH-051	SCPH-77000-12
	M-chassis	GH-052	SCPH-77000-12
	N-chassis	GH-061	SCPH-79000-12
	N-chassis	GH-062	SCPH-79000-12
	P-chassis	GH-070	SCPH-90000-12
	P-chassis	GH-071	SCPH-90000-12
	R-chassis	GH-072	SCPH-90000-12 (ROM v2.30)
	X-chassis	XPD-001	DESR-5100

Known EMCS IDs:
	00	- SONY EMCS (S EMCS)
	01	- Early "MADE IN CHINA" models - Assumed to all be Foxconn (FOXC)
	10	- S EMCS
	18	- S EMCS (PSX)
	20	- FOXC
	21	- FOXC
	30	- SuZhou MainTek (SZMT)
	40	- SONY Wuxi

List of known SSBUS I/F Controllers and their revisions:
	CXD9546R	1.2		DTL-T10000, unrefurbished.
	CXD9566R	2.0
	CXD9611R	3.0
	CXD9611AR	3.1
	CXD9611BR	3.1
	CXD9686R	Unknown
	CXD9686AR	3.1
	CXD9686BR	3.1
	CXD2955R	3.2

There's currently no known way to differentiate between a CXD9686 and CXD9611 on the software level,
with the exception of the CXD9611R.

Credits:
	This software may have been worked on mainly by me, but I had the support of other users:
		l_Oliveira, for providing a number of test reports and giving suggestions for
			its design during development.
		Berion, for the icons.
		florin and the original developers of the PS2 dumper v2 tool, since I disassembled that dumper for ideas.
		Vigilante, for corrections on the CRC values for some ROMs.
		...and various sources for their dumps from the rare and obscure PS2 models!

Old data contributers (deleted):
01	SCPH-70004	GH-032-11		Anonymous (K)	Old dump format
02	SCPH-70004	GH-035-11		Anonymous (K)	Old dump format
03	SCPH-70004	GH-035-62		Anonymous (K)	Old dump format
04	SCPH-75003	GH-041-04		Anonymous (K)	Old dump format
05	SCPH-75004	GH-040-02		Anonymous (K)	Old dump format
07	SCPH-75008	GH-040-02		denben0487	Old dump format
06	SCPH-77004	GH-052-51 (GS 1.13)	Anonymous (K)	Old dump format
08	DESR-5500	XPD-005			vitas		Old dump format
09	DTL-H10000S	GH-001			cde		Bad dump (hardware problem?)

As for those who volunteered to make the translations:
	Japanese translation *: Unassigned.
	German translation (completed): Delta_force
	French translation (completed): ShaolinAssassin
	Italian translation (completed)*: master991
	Spanish translation (completed): ElPatas
	Dutch translation (completed)*: Someone who wishes to remain anonymous.
	Portuguese translation (completed): Gillbert

	And to all users who contributed test data for the early PS2Ident builds!
