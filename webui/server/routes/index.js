const express = require('express');
const auth = require('./auth');
const db = require('./db')

const router = express.Router();

const secret = process.env.JWT_SECRET_KEY || 'change-me';
const passport = require('passport');
const JWTstrategy = require('passport-jwt').Strategy;
const ExtractJWT = require('passport-jwt').ExtractJwt;

passport.use(
  new JWTstrategy(
    {
      secretOrKey: secret,
      jwtFromRequest: ExtractJWT.fromAuthHeaderWithScheme('bearer')
    },
    async (token, done) => {
      try {
        return done(null, token.user);
      } catch (error) {
        done(error);
      }
    }
  )
);

router.use('/auth', auth);
router.use('/db', passport.authenticate('jwt', { session: false }), db);

const NFConfig = {
  // Define the route handler for NFConfig
  detail: (req, res, next) => {
    try {
      const configData = yaml.load(fs.readFileSync('/hote/test/nfconfig.yaml', 'utf8'));
      res.status(200).json(configData);
    } catch (error) {
      console.error("Error loading NFConfig:", error);
      res.status(500).json({ error: "Failed to load NFConfig data" });
    }
  }
};

// Create a custom endpoint for NFConfig
router.get('/NFConfig', NFConfig.detail);

module.exports = router;