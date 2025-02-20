// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace FlatInGamePacket.Game
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct PlayerReadyPacket : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static PlayerReadyPacket GetRootAsPlayerReadyPacket(ByteBuffer _bb) { return GetRootAsPlayerReadyPacket(_bb, new PlayerReadyPacket()); }
  public static PlayerReadyPacket GetRootAsPlayerReadyPacket(ByteBuffer _bb, PlayerReadyPacket obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool VerifyPlayerReadyPacket(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, PlayerReadyPacketVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public PlayerReadyPacket __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Uuid { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public FlatInGamePacket.Game.VehicleInformation? InitVehicleInformation { get { int o = __p.__offset(6); return o != 0 ? (FlatInGamePacket.Game.VehicleInformation?)(new FlatInGamePacket.Game.VehicleInformation()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }

  public static Offset<FlatInGamePacket.Game.PlayerReadyPacket> CreatePlayerReadyPacket(FlatBufferBuilder builder,
      int uuid = 0,
      Offset<FlatInGamePacket.Game.VehicleInformation> init_vehicle_informationOffset = default(Offset<FlatInGamePacket.Game.VehicleInformation>)) {
    builder.StartTable(2);
    PlayerReadyPacket.AddInitVehicleInformation(builder, init_vehicle_informationOffset);
    PlayerReadyPacket.AddUuid(builder, uuid);
    return PlayerReadyPacket.EndPlayerReadyPacket(builder);
  }

  public static void StartPlayerReadyPacket(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddUuid(FlatBufferBuilder builder, int uuid) { builder.AddInt(0, uuid, 0); }
  public static void AddInitVehicleInformation(FlatBufferBuilder builder, Offset<FlatInGamePacket.Game.VehicleInformation> initVehicleInformationOffset) { builder.AddOffset(1, initVehicleInformationOffset.Value, 0); }
  public static Offset<FlatInGamePacket.Game.PlayerReadyPacket> EndPlayerReadyPacket(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<FlatInGamePacket.Game.PlayerReadyPacket>(o);
  }
  public static void FinishPlayerReadyPacketBuffer(FlatBufferBuilder builder, Offset<FlatInGamePacket.Game.PlayerReadyPacket> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixedPlayerReadyPacketBuffer(FlatBufferBuilder builder, Offset<FlatInGamePacket.Game.PlayerReadyPacket> offset) { builder.FinishSizePrefixed(offset.Value); }
}


static public class PlayerReadyPacketVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*Uuid*/, 4 /*int*/, 4, false)
      && verifier.VerifyTable(tablePos, 6 /*InitVehicleInformation*/, FlatInGamePacket.Game.VehicleInformationVerify.Verify, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
