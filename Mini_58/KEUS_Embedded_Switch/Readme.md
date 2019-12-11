Taste cases

reportSwitchState: Continuous UART reply (untill getting "Expected Reply"), on the event of any switch press
28<size><commandid 1><txn><switchid><state><29>

Expected Reply:
28<size><commandid 5><txn><29>
28 02 05 01 29

configSwitch: 28<size><commandid 1><txn><no of switch><switchid><config><switchid><config>...29
28 07 01 01 02 01 01 03 01 29
28 05 01 01 01 01 02 29
reply: <28><size><uartack_id><txn><29>

getswitchstate: 28<size><commandid2><txn><29>
28 02 02 04 29
reply: <28><size><ack_type><txn><id><state>...<29>

getconfig: 28<size><commandid3><txn><29>
28 02 03 04 29
reply: <28><size><ack_type><txn><id><config>...<29>

executeSwitchState: 28<size><commandid 4><txn><no of switch><id><state><id><state>...<29>
28 05 04 01 01 01 10 29

reply: <28><size><uartack_id><txn><29>

