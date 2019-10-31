/*
v1.0
Encoder files for Mitsubishi AC
Protocol :MITSUBISHI_88AC
AC model : RKX502A007
file : mitsubishi_RKX502A
*/

const rootPath = require('app-root-path');
const IRUtils = require(rootPath + '/zigbee-coordinator/device-helpers/ir-blaster/ir-service/ir_utils');

/*

features : 
on/off
special -> high power
temp ->17-30
fan -> 0,2,3,4,5,6
swing -> on/off    //currently supporting vswing only	

power sel-not implemented

*/

const info =   {"Brand"		: "Mitshubishi",  
			    "Model"  	: "RKX502A007",
				"Protocol"  : "MITSUBISHI88AC",
				"version"	: 1,
				"filename"	: "MTSBSH_RKX502A",
				"device"	: "AC",
				};


//temp 18-30   10 ; 11 = ~10
const tempOptions = [0x07,	//18
					 0x0B,	//19
					 0x03,	//20
					 0x0D,	//21
					 0x05,	//22
					 0x09,	//23
					 0x01,	//24
					 0x0E,	//25
					 0x06,	//26
					 0x0A,	//27
					 0x02,	//28
					 0x0C,	//29
					 0x04];  //30	

//8 9=~8					
const fanOptions = {"low"  		: 0x05,
					"medium"  	: 0x01,
					"high"		: 0x06,
					"auto"		: 0x07
					};

//H 6 7=~6 ;  V 8 9=~8

const swingOptions = {	"vertical" 		: { "off" 	: [0xC0, 0xF8],
											"auto"	: [0xC0, 0xF0]},
						"horizontal"	: {"off" 	: [0x3F, 0x00],
											"auto"	: [0x3E, 0x00]},				
						};
			  

//10th byte low nibble - mode
const modeOptions = {	"cool"	: { "mode" : 0x60,
									"temp" : null,  //25
									"fan"  : null  //high
								  },
				  		"dry"	: { "mode" : 0xA0,
									"temp" : null,  //25
									"fan"  : 0x05   //auto
								  },
						"heat"	: { "mode" : 0xC0,
									"temp" : null, //t auto
									"fan"  : null  //high
								  },
				 		"fan"	: { "mode" : 0xE0,
									"temp" : 0x04,  //25
									"fan"  : null   //HIGH
								  }			  		  
					};

const updateTypesAvailable = ["ONOF"] ;
const sceneTypesAvailable =  ["ONOF"];					



class IREncoder {

    

	static getEncodedByteArray(irInfo) 
    {

    	var irData = [];

    	console.log('\n Incoming data \n')
    	console.log(irInfo);

    	var 
    		temp = irInfo.temperature ? (irInfo.temperature -18) : 6,
    		fan	= irInfo.fan ? irInfo.fan : 0,
    		power = irInfo.power ? 1 :0,
      		swing_v = irInfo.swing.vertical ? irInfo.swing.vertical : "off",
    		swing_h = irInfo.swing.horizontal ? irInfo.swing.horizontal : "off",
    		mode = irInfo.mode ? irInfo.mode : 0,
    		special = irInfo.special ? 1 :0,
    		type = irInfo.updateType  ? irInfo.updateType : [],
    		configId = irInfo.configId  ? irInfo.configId : [],   		
    		noOfSignal = irInfo.no_of_signal ? irInfo.no_of_signal : 1,

    		i=0, j = 0, x = 0,
    		checkSum = 0,
    		byt = 0;



    	if(noOfSignal > 0)
    	{
    		//no of signals
    		irData[i] = noOfSignal;
			i++;
    	}	

    	while(j < noOfSignal)	
    	{
    		

    			//config Id
    			irData[i] = configId;
				i++;

				//pos of length
				x = i;
				i++;

				checkSum = 0;


				console.log('x');
				console.log(x);


				irData[i] = 0x4A;
				i++;
	
				irData[i] = 0x75;
				i++;		

				irData[i] = 0xC3;
				i++;
	
				irData[i] = 0x64;
				i++;		

				irData[i] = 0x9B;
				i++;		

				// 6 AND 7
				irData[i] = swingOptions["vertical"][swing_v][0] + swingOptions["horizontal"][swing_h][0];
				i++;	

				irData[i] = IRUtils.binaryCompliment({number : irData[i-1], pow : 8});
				i++;
				//

				//8 and 9	
				irData[i] = swingOptions["vertical"][swing_v][1] + fanOptions[fan] + swingOptions["horizontal"][swing_h][1];
				i++;	

				irData[i] = IRUtils.binaryCompliment({number : irData[i-1], pow : 8});;
				i++;
				//
	
				//10 and 11	
				//default mode cold since option not provided yet		
				irData[i] = modeOptions[mode]["mode"] +  (power == 1 ? 0x00 : 0x10 ) + tempOptions[temp];
				i++;	

				irData[i] = IRUtils.binaryCompliment({number : irData[i-1], pow : 8});;
				i++;		

				irData[x] = i-x-1;
			
				j++;

		}		

 		console.log('\n Formed data');
 		var stringer = "[";
		irData.forEach(function(element){
    		stringer += ('0x' +element.toString(16) + ", ");
		});
		stringer += "]";
		console.log(stringer);

		return irData;


	}

	//Mitsubishi
	
	static getSceneArray(irInfo) 
	{
	
		var configId = irInfo.configId  ? irInfo.configId : 0;
	
		if(irInfo.power)
		{
			irInfo.updateType = sceneTypesAvailable; 
			irInfo.no_of_signal = sceneTypesAvailable.length;
		}
		else 
		{
			irInfo.updateType = ["ONOF"]; 
			irInfo.no_of_signal = 1;
		}
	
	
		if(configId)
		{
			return this.getEncodedByteArray(irInfo);
		}
	
		//return [];
	}
	
	static getSignalArray(irInfo) 
	{
		var	configId = irInfo.configId  ? irInfo.configId : 0;
	
		if(updateTypesAvailable.indexOf(irInfo.updateType[0]) < 0)
		{
			irInfo.updateType = ["ONOF"]; 
		}
		
		irInfo.no_of_signal = 1;
	
		if(configId)
		{
			return this.getEncodedByteArray(irInfo);
		}
	
		//return
	}
	
	static getConfigInfo()
	{
		return {
			configData : [8000,3200,1600,400, 400,400,1200,400,38000],
		    configFrame : [0x53]
		}
	
		//return [];
	}

	static getTempLevels()
	{
		return {"low" : 18,
				"high" : 30 
				};
	}

	static getFanLevels()
	{ 
		return Object.keys(fanOptions) ;
	}

	static getSwingLevels()
	{
		return {
			horizontal 	: Object.keys(swingOptions.horizontal),
			vertical 	: Object.keys(swingOptions.vertical) 
		}
	}

	static getModes()
	{
		return Object.keys(modeOptions);
	}

	static getInfo()
	{
		return info;
	}

}

module.exports = IREncoder;


