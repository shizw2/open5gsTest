const mongoose = require('mongoose');
const Schema = mongoose.Schema;
require('mongoose-long')(mongoose);

const Imeicheck = new Schema({

  schema_version: {
    $type: Number,
    default: 1  // Current Schema Version
  }, 
  imei: { $type: String, unique: true, required: true },
  reason: Number,
  status: Number,
  checkflag:Boolean,
  bindimsi:[{
  msisdn: String ,
  imsi: String 
  }] 
}, { typeKey: '$type' });

module.exports = mongoose.model('Imeicheck', Imeicheck);
