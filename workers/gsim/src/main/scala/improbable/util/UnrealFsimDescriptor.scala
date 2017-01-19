package improbable.util

import java.io.File
import java.nio.file.Path

import improbable.os.OperatingSystemUtil
import improbable.os.OperatingSystemUtil.OperatingSystem
import improbable.fapi.engine.{DownloadableEngineDescriptor, EngineStartConfig}
import improbable.bridgesettings.UnrealFsimBridgeSettings

class UnrealFsimDescriptor extends DownloadableEngineDescriptor {
  def enginePlatform = UnrealFsimBridgeSettings.UNREAL_FSIM_TYPE

  override def startCommand(config: EngineStartConfig, enginePath: Path): Seq[String] = {
    val result = Seq(makeExecutablePath(enginePath).toString)
//      config.receptionistIp,
//      "7777",
//      config.engineId)
    print(result)
    result
  }

  private def makeExecutablePath(startPath: Path): Path = {
    val operatingSystem = OperatingSystemUtil.currentOperatingSystem
    val executableName = s"$enginePlatform@$operatingSystem"
    val localExecutablePath = operatingSystem match {
      case OperatingSystem.Linux =>
        executableName

      case OperatingSystem.Mac =>
        s"$executableName.app/Contents/MacOS/$executableName"

      case OperatingSystem.Windows =>
        s"$executableName.exe"
    }
    val absoluteExecutablePath = startPath.resolve(localExecutablePath).toAbsolutePath

    ensureFileIsExecutable(absoluteExecutablePath.toFile)
    absoluteExecutablePath
  }

  private def ensureFileIsExecutable(file: File): Unit = {
    file.setExecutable(true)
    file.setReadable(true)
  }
}

object UnrealFsimDescriptor extends UnrealFsimDescriptor