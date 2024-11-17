
const Ommlog=require('./ommlog.js');
const Subscriber=require('./subscriber.js');

exports.fetchommlogs=function(res) {
  //console.log('> Ready on fetchNfStatus');
  Ommlog.find({}, function(err, ommlogs) {
    if (err) return console.error(err);
    console.log('> Found '+ommlogs.length+' ommlogs');
    //console.log(ommlogs);
    res.setHeader('Content-Type', 'application/json');
    res.end(JSON.stringify(ommlogs));
  });
}
exports.fetchsubscribers=function(res) {
  //console.log('> Ready on fetchNfStatus');
  Subscriber.find({}, function(err, subscribers) {
    if (err) return console.error(err);
    console.log('> Found '+subscribers.length+' Subscribers');
    //console.log(subscribers);
    res.setHeader('Content-Type', 'application/json');
    res.end(JSON.stringify(subscribers));
  });
}
//module.exports = fetchdb;