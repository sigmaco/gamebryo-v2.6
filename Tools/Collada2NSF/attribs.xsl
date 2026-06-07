<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:col="http://www.collada.org/2005/11/COLLADASchema">

  <!-- Attribute List -->
  <xsl:template name="GlobalAttributeList">
    <xsl:param name="MaterialId"/>

    <!-- Gather attributes into variable -->
    <xsl:variable name="GlobalAttributes">
      <xsl:for-each select="col:newparam">
        <xsl:apply-templates select=".">
          <xsl:with-param name="MaterialId" select="$MaterialId"/>
          <xsl:with-param name="DoGlobals" select="true()"/>
        </xsl:apply-templates>
      </xsl:for-each>
    </xsl:variable>

    <!-- Only display sectionf if data was found -->
    <xsl:if test="not($GlobalAttributes='')">
      <xsl:value-of select="concat($Tab, 'Global_Attributes', '&#xA;')"/>
      <xsl:value-of select="concat($Tab, '{', '&#xA;')"/>
      <xsl:value-of select="$GlobalAttributes"/>
      <xsl:value-of select="concat($Tab, '}', '&#xA;')"/>
    </xsl:if>

  </xsl:template>
  
  <!-- Attribute List -->
  <xsl:template name="AttributeList">
    <xsl:param name="MaterialId"/>

    <!-- Gather attributes into variable -->
    <xsl:variable name="Attributes">
      <xsl:for-each select="col:newparam">
        <xsl:apply-templates select=".">
          <xsl:with-param name="MaterialId" select="$MaterialId"/>
          <xsl:with-param name="DoGlobals" select="false()"/>
        </xsl:apply-templates>
      </xsl:for-each>
    </xsl:variable>
    
    <!-- Only display sectionf if data was found -->
    <xsl:if test="not($Attributes='')">
      <xsl:value-of select="concat($Tab, 'Attributes', '&#xA;')"/>
      <xsl:value-of select="concat($Tab, '{', '&#xA;')"/>
      <xsl:value-of select="$Attributes"/>
      <xsl:value-of select="concat($Tab, '}', '&#xA;')"/>
    </xsl:if>
  </xsl:template>

  <!-- newparam -->
  <xsl:template match="col:newparam">
    <xsl:param name="MaterialId"/>
    <xsl:param name="DoGlobals"/>

    <!-- If min/max annotations are present then there is a range -->
    <xsl:variable name="Min" select="col:annotate[@name='min']"/>
    <xsl:variable name="Max" select="col:annotate[@name='max']"/>
    <xsl:variable name="UIMin" select="col:annotate[@name='UIMin']"/>
    <xsl:variable name="UIMax" select="col:annotate[@name='UIMax']"/>

    <!-- Collect information about the type of attribute -->
    <xsl:variable name="Attrib_Bool" select="count(col:bool)"/>
    <xsl:variable name="Attrib_UInt" select="count(col:int)"/>
    <xsl:variable name="Attrib_Float" select="count(col:float)"/>
    <xsl:variable name="Attrib_Point2" select="count(col:float2)"/>
    <xsl:variable name="Attrib_Point3" select="count(col:float3)"/>
    <xsl:variable name="Attrib_Point4" select="count(col:float4)"/>
    <xsl:variable name="Attrib_Matrix3" select="count(col:float3x3)"/>
    <xsl:variable name="Attrib_Transform" select="count(col:float4x4)"/>
    <xsl:variable name="Attrib_Texture" select="count(col:surface)"/>
    <xsl:variable name="VarType" select="translate(col:annotate[@name='VarType']/col:string, $l, $u)"/>
    <xsl:variable name="HasColorAttr" select="col:annotate[@name='color']/col:bool"/>
    <xsl:variable name="IsGlobal" select="$VarType='GLOBAL'"/>
    <xsl:variable name="IsObject" select="$VarType='OBJECT'"/>

    <xsl:variable name="Skip" select="($DoGlobals and not($IsGlobal)) or (not($DoGlobals) and $IsGlobal) or $IsObject"/>
    <xsl:if test="not($Skip)">

      <!-- Build the attribute -->
      <xsl:variable name="Attribute">
        <xsl:choose>
          <xsl:when test='$Attrib_Bool=1'>
            <xsl:value-of select="concat($Tabs2, 'Attrib_Bool ', @sid)"/>
            <xsl:call-template name="ArtistFlag"/>
            <xsl:call-template name="ResolveValue">
              <xsl:with-param name="MaterialId" select="$MaterialId"/>
              <xsl:with-param name="Ref" select="@sid"/>
              <xsl:with-param name="DefaultValue" select="col:bool"/>
              <xsl:with-param name="NeedsConversion">false</xsl:with-param>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test='$Attrib_UInt=1'>
            <xsl:value-of select="concat($Tabs2, 'Attrib_UInt ', @sid)"/>
            <xsl:call-template name="ArtistFlag"/>
            <xsl:call-template name="ResolveValue">
              <xsl:with-param name="MaterialId" select="$MaterialId"/>
              <xsl:with-param name="Ref" select="@sid"/>
              <xsl:with-param name="DefaultValue" select="col:int"/>
              <xsl:with-param name="NeedsConversion">false</xsl:with-param>
            </xsl:call-template>
            <xsl:choose>
              <xsl:when test="$Min and $Max">
                <xsl:text> [</xsl:text>
                <xsl:value-of select="$Min/col:int"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="$Max/col:int"/>
                <xsl:text>]</xsl:text>
              </xsl:when>
              <xsl:when test="$UIMin and $UIMax">
                <xsl:text> [</xsl:text>
                <xsl:value-of select="$UIMin/col:int"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="$UIMax/col:int"/>
                <xsl:text>]</xsl:text>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
          <xsl:when test='$Attrib_Float=1'>
            <xsl:value-of select="concat($Tabs2, 'Attrib_Float ', @sid)"/>
            <xsl:call-template name="ArtistFlag"/>
            <xsl:call-template name="ResolveValue">
              <xsl:with-param name="MaterialId" select="$MaterialId"/>
              <xsl:with-param name="Ref" select="@sid"/>
              <xsl:with-param name="DefaultValue" select="col:float"/>
              <xsl:with-param name="NeedsConversion">true</xsl:with-param>
            </xsl:call-template>
            <xsl:choose>
              <xsl:when test="$Min and $Max">
                <xsl:text> [</xsl:text>
                <xsl:call-template name="EmitFloat">
                  <xsl:with-param name="Value" select="$Min/col:float"/>
                </xsl:call-template>
                <xsl:text> </xsl:text>
                <xsl:call-template name="EmitFloat">
                  <xsl:with-param name="Value" select="$Max/col:float"/>
                </xsl:call-template>
                <xsl:text>]</xsl:text>
              </xsl:when>
              <xsl:when test="$UIMin and $UIMax">
                <xsl:text> [</xsl:text>
                <xsl:call-template name="EmitFloat">
                  <xsl:with-param name="Value" select="$UIMin/col:float"/>
                </xsl:call-template>
                <xsl:text> </xsl:text>
                <xsl:call-template name="EmitFloat">
                  <xsl:with-param name="Value" select="$UIMax/col:float"/>
                </xsl:call-template>
                <xsl:text>]</xsl:text>
              </xsl:when>
            </xsl:choose>
          </xsl:when>
          <xsl:when test='$Attrib_Point2=1'>
            <xsl:value-of select="concat($Tabs2, 'Attrib_Point2 ', @sid)"/>
            <xsl:call-template name="ArtistFlag"/>
            <xsl:call-template name="ResolveValue">
              <xsl:with-param name="MaterialId" select="$MaterialId"/>
              <xsl:with-param name="Ref" select="@sid"/>
              <xsl:with-param name="DefaultValue" select="col:float2"/>
              <xsl:with-param name="NeedsConversion">true</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="ConvertFloatRange">
              <xsl:with-param name="Min" select="$Min"/>
              <xsl:with-param name="Max" select="$Max"/>
              <xsl:with-param name="UIMin" select="$UIMin"/>
              <xsl:with-param name="UIMax" select="$UIMax"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test='$Attrib_Point3=1'>
            <xsl:variable name="IsColor" select="col:semantic='DIFFUSE' or col:semantic='SPECULAR' or col:semantic='AMBIENT' or col:semantic='COLOR'"/>
            <xsl:choose>
              <xsl:when test="$IsColor=1 or $HasColorAttr">
                <xsl:value-of select="concat($Tabs2, 'Attrib_Color ', @sid)"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="concat($Tabs2, 'Attrib_Point3 ', @sid)"/>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:call-template name="ArtistFlag"/>
            <xsl:call-template name="ResolveValue">
              <xsl:with-param name="MaterialId" select="$MaterialId"/>
              <xsl:with-param name="Ref" select="@sid"/>
              <xsl:with-param name="DefaultValue" select="col:float3"/>
              <xsl:with-param name="NeedsConversion">true</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="ConvertFloatRange">
              <xsl:with-param name="Min" select="$Min"/>
              <xsl:with-param name="Max" select="$Max"/>
              <xsl:with-param name="UIMin" select="$UIMin"/>
              <xsl:with-param name="UIMax" select="$UIMax"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test='$Attrib_Point4=1'>
            <xsl:variable name="IsColor" select="col:semantic='DIFFUSE' or col:semantic='SPECULAR' or col:semantic='AMBIENT' or col:semantic='COLOR'"/>
            <xsl:choose>
              <xsl:when test="$IsColor=1 or $HasColorAttr">
                <xsl:value-of select="concat($Tabs2, 'Attrib_Color ', @sid)"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="concat($Tabs2, 'Attrib_Point4 ', @sid)"/>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:call-template name="ArtistFlag"/>
            <xsl:call-template name="ResolveValue">
              <xsl:with-param name="MaterialId" select="$MaterialId"/>
              <xsl:with-param name="Ref" select="@sid"/>
              <xsl:with-param name="DefaultValue" select="col:float4"/>
              <xsl:with-param name="NeedsConversion">true</xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="ConvertFloatRange">
              <xsl:with-param name="Min" select="$Min"/>
              <xsl:with-param name="Max" select="$Max"/>
              <xsl:with-param name="UIMin" select="$UIMin"/>
              <xsl:with-param name="UIMax" select="$UIMax"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test='$Attrib_Matrix3=1'>
            <xsl:value-of select="concat($Tabs2, 'Attrib_Matrix3 ', @sid)"/>
            <xsl:call-template name="ArtistFlag"/>
            <xsl:call-template name="ResolveValue">
              <xsl:with-param name="MaterialId" select="$MaterialId"/>
              <xsl:with-param name="Ref" select="@sid"/>
              <xsl:with-param name="DefaultValue" select="col:float3x3"/>
              <xsl:with-param name="NeedsConversion">true</xsl:with-param>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test='$Attrib_Transform=1'>
            <xsl:value-of select="concat($Tabs2, 'Attrib_Transform ', @sid)"/>
            <xsl:call-template name="ArtistFlag"/>
            <xsl:call-template name="ResolveValue">
              <xsl:with-param name="MaterialId" select="$MaterialId"/>
              <xsl:with-param name="Ref" select="@sid"/>
              <xsl:with-param name="DefaultValue" select="col:float4x4"/>
              <xsl:with-param name="NeedsConversion">true</xsl:with-param>
            </xsl:call-template>
          </xsl:when>
          <xsl:when test='$Attrib_Texture=1'>
            <xsl:variable name="NTMFlag" select="normalize-space(col:annotate[@name='NTM']/col:string)"/>
            <xsl:variable name="NTMIndex" select="normalize-space(col:annotate[@name='NTMIndex']/col:int)"/>
            <xsl:if test="$NTMFlag='' or $NTMFlag='shader'">
              <xsl:value-of select="concat($Tabs2, 'Attrib_Texture ', @sid)"/>
              <xsl:call-template name="ArtistFlag"/>
              <xsl:choose>
                <xsl:when test="$NTMIndex=''">
                  <xsl:text></xsl:text>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="concat($NTMIndex, ' ')"/>
                </xsl:otherwise>
              </xsl:choose>
              <xsl:text>&quot;</xsl:text>
              <xsl:call-template name="ResolveValue">
                <xsl:with-param name="MaterialId" select="$MaterialId"/>
                <xsl:with-param name="Ref" select="@sid"/>
                <xsl:with-param name="DefaultValue" select="col:surface"/>
                <xsl:with-param name="NeedsConversion" select="false"/>
              </xsl:call-template>
              <xsl:text>&quot;</xsl:text>
            </xsl:if>
          </xsl:when>
        </xsl:choose>
      </xsl:variable>

      <xsl:if test="normalize-space($Attribute)!=''">
        <xsl:value-of select="concat($Tabs2, '// ', @sid, ':', col:semantic, '&#xA;')"/>
        <xsl:value-of select="$Attribute"/>
        <xsl:text>&#xA;</xsl:text>
      </xsl:if>

    </xsl:if>
  </xsl:template>

  <!-- ResolveValue -->
  <xsl:template name="ResolveValue">
    <xsl:param name="NeedsConversion"/>
    <xsl:param name="MaterialId"/>
    <xsl:param name="Ref"/>
    <xsl:param name="DefaultValue"/>
    <xsl:variable name="Material" select="/col:COLLADA/col:library_materials/col:material[@id = $MaterialId]"/>
    <xsl:variable name="URL" select="concat('#', ancestor::col:effect/@id)"/>
    <xsl:variable name="Instance" select="$Material/col:instance_effect[@url = $URL]"/>
    <xsl:variable name="Param" select="$Instance/col:setparam[@ref = $Ref]"/>
    <xsl:choose>
      <!-- When this is a surface node... -->
      <xsl:when test="name($DefaultValue)='surface'">
        <xsl:choose>
          <!-- When there is a material replacement -->
          <xsl:when test="$Param!=''">
            <xsl:variable name="ImageId" select="$Param/col:surface/col:init_from"/>
            <xsl:variable name="LibImage" select="/col:COLLADA/col:library_images/col:image[@id = $ImageId]/col:init_from"/>
            <!-- When there is a value found in the image library, 
              use that, otherwise use ImageId directly -->
            <xsl:choose>
              <xsl:when test="$LibImage!=''">
                <xsl:value-of select="$LibImage"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$ImageId"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>

          <!-- Otherwise, there was no material replacement
          so we use the value directly from the effect newparam node
          -->
          <xsl:otherwise>
            <xsl:variable name="ImageId" select="$DefaultValue/col:init_from"/>
            <xsl:choose>

              <!-- If image id is specified backtrack... -->
              <xsl:when test="$ImageId!=''">
                <xsl:variable name="pid" select="../col:image[@id = $ImageId]/col:init_from"/>
                <xsl:variable name="eid" select="../../col:image[@id = $ImageId]/col:init_from"/>
                <xsl:variable name="lid" select="/col:COLLADA/col:library_images/col:image[@id = $ImageId]/col:init_from"/>
                <xsl:choose>
                  <xsl:when test="$pid!=''">
                    <xsl:variable name="eid2" select="../../col:image[@id = $pid]/col:init_from"/>
                    <xsl:choose>
                      <xsl:when test="$eid2!=''">
                        <xsl:variable name="lid2" select="/col:COLLADA/col:library_images/col:image[@id = $eid2]/col:init_from"/>
                        <xsl:choose>
                          <xsl:when test="$lid2!=''">
                            <xsl:value-of select="$lid2"/>
                          </xsl:when>
                          <xsl:otherwise>
                            <xsl:value-of select="$eid2"/>
                          </xsl:otherwise>
                        </xsl:choose>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="$pid"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </xsl:when>
                  <xsl:when test="$eid!=''">
                    <xsl:variable name="lid2" select="/col:COLLADA/col:library_images/col:image[@id = $eid]/col:init_from"/>
                    <xsl:choose>
                      <xsl:when test="$lid2!=''">
                        <xsl:value-of select="$lid2"/>
                      </xsl:when>
                      <xsl:otherwise>
                        <xsl:value-of select="$eid"/>
                      </xsl:otherwise>
                    </xsl:choose>
                  </xsl:when>
                  <xsl:when test="$lid!=''">
                    <xsl:value-of select="$lid"/>
                  </xsl:when>
                  <xsl:otherwise>
                    <xsl:value-of select="$ImageId"/>
                  </xsl:otherwise>
                </xsl:choose>
              </xsl:when>

              <!-- Otherwise, there was no image id so use resource name -->
              <xsl:otherwise>
                <xsl:value-of select="col:annotate[@name='ResourceName']/col:string"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>
     </xsl:when>
      <xsl:otherwise>
        <xsl:variable name="Value" select="$Param/*[name() = name($DefaultValue)]"/>
        <xsl:choose>
          <xsl:when test="$Value=''">
            <xsl:choose>
              <xsl:when test="$NeedsConversion='true'">
                <xsl:call-template name="ConvertFloats">
                  <xsl:with-param name="Value" select="$DefaultValue"/>
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$DefaultValue"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:choose>
              <xsl:when test="$NeedsConversion='true'">
                <xsl:call-template name="ConvertFloats">
                  <xsl:with-param name="Value" select="$Value"/>
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$Value"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Emits a single float -->
  <xsl:template name="EmitFloat">
    <xsl:param name="Value"/>
    
    <xsl:choose>
      <xsl:when test="$Value='' or $Value='0'">
        <xsl:text>0.0</xsl:text>
      </xsl:when>
      <xsl:when test="not(contains($Value, '.'))">
        <xsl:value-of select="concat($Value, '.0')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$Value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Converts a string of floats to a comma separated list of floats -->
  <xsl:template name="ConvertFloats">
    <xsl:param name="Value"/>
    <xsl:choose>
      <xsl:when test="contains($Value, ' ')">
        <xsl:call-template name="ConvertFloatsRecursive">
          <xsl:with-param name="Value">
            <xsl:value-of select="concat(normalize-space($Value), ' ')"/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="EmitFloat">
          <xsl:with-param name="Value">
            <xsl:value-of select="$Value"/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="ConvertFloatsRecursive">
    <xsl:param name="Value"/>
    <xsl:variable name="Current" select="substring-before($Value, ' ')"/>
    <xsl:if test="$Current!=''">
      <xsl:call-template name="EmitFloat">
        <xsl:with-param name="Value" select="$Current"/>
      </xsl:call-template>
      <xsl:variable name="Rest" select="substring($Value, string-length($Current)+2)"/>
      <xsl:if test="contains($Rest, ' ')">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:call-template name="ConvertFloatsRecursive">
        <xsl:with-param name="Value" select="$Rest"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>
  
  <!-- Emit artist flag or hidden -->
  <xsl:template name="ArtistFlag">
    <xsl:variable name="Hidden" select="translate(col:annotate[@name='hidden']/col:string, $l, $u)"/>
    <xsl:variable name="UIWidget" select="translate(col:annotate[@name='UIWidget']/col:string, $l, $u)"/>
    <xsl:choose>
      <xsl:when test="$Hidden='TRUE' or $UIWidget='NONE'">
        <xsl:text> Hidden </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text> Artist </xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Emit artist flag or hidden -->
  <xsl:template name="ConvertFloatRange">
    <xsl:param name="Min"/>
    <xsl:param name="Max"/>
    <xsl:param name="UIMin"/>
    <xsl:param name="UIMax"/>
    <xsl:choose>
      <xsl:when test="$Min and $Max">
        <xsl:text> [ (</xsl:text>
        <xsl:call-template name="ConvertFloats">
          <xsl:with-param name="Value" select="$Min"/>
        </xsl:call-template>
        <xsl:text>) (</xsl:text>
        <xsl:call-template name="ConvertFloats">
          <xsl:with-param name="Value" select="$Max"/>
        </xsl:call-template>
        <xsl:text>) ]</xsl:text>
      </xsl:when>
      <xsl:when test="$UIMin and $UIMax">
        <xsl:text> [ (</xsl:text>
        <xsl:call-template name="ConvertFloats">
          <xsl:with-param name="Value" select="$UIMin"/>
        </xsl:call-template>
        <xsl:text>) (</xsl:text>
        <xsl:call-template name="ConvertFloats">
          <xsl:with-param name="Value" select="$UIMax"/>
        </xsl:call-template>
        <xsl:text>) ]</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
