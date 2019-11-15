/*
v1.0
Encoder files for daikin AC
Protocol : Fujitsu128_56AC
AC model : O General_AR-RY180
*/

var ir_out2 = [];

const tempOptions = {
    "18": 0x20,
    "19": 0x30,
    "20": 0x40,
    "21": 0x50,
    "22": 0x60,
    "23": 0x70,
    "24": 0x80,
    "25": 0x90,
    "26": 0xA0,
    "27": 0xB0,
    "28": 0xC0,
    "29": 0xD0,
    "30": 0xE0
};

const fanOptions = {
    "quiet": 0x04,
    "low": 0x03,
    "med": 0x02,
    "high": 0x01,
    "auto": 0x0
};

function setSwing(hsw, vsw) {
    ir_out2[11] &= 0x0F;
    if (vsw == 1 && hsw == 1) {
        ir_out2[11] |= 0x30;
    }
    else if (vsw == 1) {
        ir_out2[11] |= 0x10;
    }
    else if (hsw == 1) {
        ir_out2[11] |= 0x20;
    }
}

const modeOptions = {
    "auto": 0x0,
    "cool": 0x01,
    "dry": 0x02,
    "fan": 0x03
}


function setPower(power, change) {
    ir_out2[9] &= 0xF0;
    if (power && (change == 1)) {
        ir_out2[9] |= 0x01;
    }
    if (!power) {
        ir_out2[0] = 0x38;
        ir_out2[1] = 0x14;
        ir_out2[2] = 0x63;
        ir_out2[3] = 0x0;
        ir_out2[4] = 0x10;
        ir_out2[5] = 0x10;
        ir_out2[6] = 0x02;
        ir_out2[7] = 0xFD;
    }
}

function checkSum() {
    var sum = 0x0;
    for (var k=8; k<15; k++) {

        sum += ir_out2[k];
    }
    sum &= 0x0FF;
    //console.log(sum);

    ir_out2[16] = (0xE0 - sum);    
}

//LSB8 to MSB converter
function lsb8_to_msb() {
    var reverse_num = 0;
    var val = 0;
    for (var j = 1; j < 17; j++) {
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

    ir_out2[1] = 0x14;
    ir_out2[2] = 0x63;
    ir_out2[3] = 0x0;
    ir_out2[4] = 0x10;
    ir_out2[5] = 0x10;
    ir_out2[6] = 0xFE;
    ir_out2[7] = 0x09;
    ir_out2[8] = 0x30;
    ir_out2[10] = 0x0;
    ir_out2[11] = 0x0;
    ir_out2[12] = 0x0;
    ir_out2[13] = 0x0;
    ir_out2[14] = 0x0;
    ir_out2[15] = 0x20;
    ir_out2[16] = 0x0;

    ir_out2[9] |= tempOptions[irInfo.temp.toString()];

    //console.log(tempOptions[irInfo.temp.toString()]);

    ir_out2[11] |= fanOptions[irInfo.fan.toString()];

    setSwing(irInfo.hsw, irInfo.vsw);

    ir_out2[10] |= modeOptions[irInfo.mode.toString()];

    ir_out2[0] = 0x80; // Length to consider

    setPower(irInfo.power, irInfo.change);

    checkSum();

    lsb8_to_msb();

    

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
xyz.temp = 30;
xyz.fan = "auto";
xyz.hsw = 0;
xyz.vsw = 0;
xyz.mode = "auto";
xyz.change = 2;

ir_generator(xyz);