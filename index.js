const processVolumes = require('./build/Release/process-volumes');

module.exports = {
  getProcessVolumes: function () {
    return processVolumes.getProcessVolumes();
  }
};
