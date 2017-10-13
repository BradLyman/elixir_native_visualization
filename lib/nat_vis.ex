defmodule NatVis do
  @moduledoc """
  Documentation for ElixirNativeVisualization.
  """

  @on_load :load_nif

  def load_nif do
    :erlang.load_nif("_build/libNatVis", 0)
  end

  def start, do: raise "not implemented"

  def stop(_app), do: raise "not implemented"
end

