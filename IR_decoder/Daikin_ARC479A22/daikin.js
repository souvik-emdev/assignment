/*
v1.0
Encoder files for daikin AC
Protocol : SANYO152AC
AC model : daikin ARC479A22
*/
var ir_out2 = [];

const tempOptions = {
    "18": 0x24,
    "19": 0x26,
    "20": 0x28,
    "21": 0x2A,
    "22": 0x2C,
    "23": 0x2E,
    "24": 0x30,
    "25": 0x32,
    "26": 0x34,
    "27": 0x36,
    "28": 0x38,
    "29": 0x3A,
    "30": 0x3C,
    "31": 0x3E,
    "32": 0x40
};

const fanOptions = {
    "sleep": 0xB0,
    "1": 0x30,
    "2": 0x40,
    "3": 0x50,
    "4": 0x60,
    "5": 0x70,
    "auto": 0xA0
};

const swing = {
    "1": 0x0F,
    "0": 0x00
};

const power = {
    "1": 0x01,
    "0": 0x00
};

function setmode(mode) {

    switch (mode) {

        case 1:             //cool
            ir_out2[6] = 0x30;
            break;
        case 2:             //dry
            ir_out2[6] = 0x20;
            ir_out2[7] = 0xC0;
            ir_out2[9] &= 0x0F;
            ir_out2[9] |= 0xA0;
            break;
        case 3:             //fan
            ir_out2[6] = 0x60;
            ir_out2[7] = 0x32;
            break;

    }
}

function summation() {
    var sum = 0x0;
    for (var k=1; k<19; k++) {

        sum += ir_out2[k];
    }
    ir_out2[19] = sum;    
}

//LSB8 to MSB converter
function lsb8_to_msb() {
    var reverse_num = 0;
    var val = 0;
    for (var j = 1; j < 20; j++) {
        for (var i = 0; i < 8; i++) {
            val = (ir_out2[j] & (1 << i));
            if (val)
                reverse_num |= (1 << ((8 - 1) - i));
        }
        ir_out2[j] = reverse_num;
        val = 0;
        reverse_num = 0;
    }
}



function ir_generator(irInfo) {
    //Submit data in the following format to decode
    //Power[0/1],Temperature[18-32],FanMode[sleep,1-5,auto],HSW[0/1],VSW[0/1],Mode[1-3]
    //Mode: Fan=3, Cool=1, Dry=2,

    ir_out2[1] = 0x11;
    ir_out2[2] = 0xDA;
    ir_out2[3] = 0x27;
    ir_out2[4] = 0x0;
    ir_out2[5] = 0x0;
    ir_out2[8] = 0x0;
    ir_out2[11] = 0x0;
    ir_out2[12] = 0x06;
    ir_out2[13] = 0x60;
    ir_out2[14] = 0x0;
    ir_out2[15] = 0x0;
    ir_out2[16] = 0xC2;
    ir_out2[17] = 0x80;
    ir_out2[18] = 0x0;
    //ir_out2[19] = 0x0;

    ir_out2[7] = tempOptions[irInfo.temp.toString()];

    //console.log(tempOptions[irInfo.temp.toString()]);

    ir_out2[9] = fanOptions[irInfo.fan.toString()];
    ir_out2[9] |= swing[irInfo.vsw.toString()];
    ir_out2[10] = swing[irInfo.hsw.toString()];

    setmode(irInfo.mode);

    ir_out2[6] |= power[irInfo.power.toString()];



    // settemp(irInfo.temp);
    // setfan(irInfo.fan);
    // setswing(irInfo.hsw, irInfo.vsw);
    // setmode(irInfo.mode);

    summation();

    lsb8_to_msb();

    ir_out2[0] = 0x98; // Length to consider

    //console.log(ir_out2);

    console.log('\nFormed Data');
    var stringer = "[";
    ir_out2.forEach(function (element) {
        stringer += ('0x' + element.toString(16) + ", ");
    });
    stringer += "]";
    console.log(stringer);

}

var xyz = {};
xyz.power = 1;
xyz.temp = 19;
xyz.fan = "auto";
xyz.hsw = 1;
xyz.vsw = 1;
xyz.mode = 3;

ir_generator(xyz);